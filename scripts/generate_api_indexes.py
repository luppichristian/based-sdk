#!/usr/bin/env python3

from __future__ import annotations

import argparse
from pathlib import Path
import re
import sys


DEFAULT_MODULES = ("core", "gfx")
API_LINE_PATTERNS = (
    re.compile(r"^\s*#\s*include\b"),
    re.compile(r"^\s*#\s*define\b"),
    re.compile(r"^\s*typedef\b"),
    re.compile(r"^\s*func\b"),
)


def _is_api_line(line: str) -> bool:
    return any(pattern.match(line) for pattern in API_LINE_PATTERNS)


def _format_rel_path(path: Path, module_dir: Path) -> str:
    return str(path.relative_to(module_dir)).replace("/", "\\")


def _index_header(header_path: Path, module_dir: Path) -> str:
    rel_path = _format_rel_path(header_path, module_dir)
    entries: list[str] = []
    with header_path.open("r", encoding="utf-8") as file_handle:
        for line_number, line in enumerate(file_handle, start=1):
            stripped = line.strip()
            if stripped and _is_api_line(stripped):
                entries.append(f"{line_number}: {stripped}")

    lines = [f"=== {rel_path} ===", *entries, ""]
    return "\n".join(lines)


def _build_module_index(module_dir: Path) -> str:
    include_dir = module_dir / "include"
    if not include_dir.is_dir():
        raise FileNotFoundError(f"Missing include directory: {include_dir}")

    header_paths = sorted(include_dir.rglob("*.h"))
    indexed_headers = [
        _index_header(header_path, module_dir) for header_path in header_paths
    ]
    content = "\n".join(indexed_headers).rstrip() + "\n"
    return content


def _process_module(module_name: str, repo_root: Path, check: bool) -> int:
    module_dir = repo_root / "modules" / module_name
    if not module_dir.is_dir():
        print(f"[error] Module directory does not exist: {module_dir}", file=sys.stderr)
        return 1

    index_path = module_dir / "api_index.txt"
    generated_content = _build_module_index(module_dir)
    existing_content = ""
    if index_path.is_file():
        existing_content = index_path.read_text(encoding="utf-8")

    is_stale = existing_content != generated_content

    if check:
        if is_stale:
            print(f"[stale] {index_path}")
            return 1

        print(f"[ok] {index_path}")
        return 0

    if is_stale:
        index_path.write_text(generated_content, encoding="utf-8")
        print(f"[regenerated] {index_path}")
    else:
        print(f"[up-to-date] {index_path}")

    return 0


def parse_args() -> argparse.Namespace:
    script_dir = Path(__file__).resolve().parent
    default_root = script_dir.parent

    parser = argparse.ArgumentParser(
        description="Generate module api_index.txt files by parsing public headers"
    )
    parser.add_argument(
        "--root",
        type=Path,
        default=default_root,
        help="Repository root (defaults to script parent directory)",
    )
    parser.add_argument(
        "--modules",
        nargs="+",
        default=list(DEFAULT_MODULES),
        help="Module names to process",
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="Check if indexes are up to date without writing files",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    repo_root = args.root.resolve()

    exit_code = 0
    for module_name in args.modules:
        module_exit = _process_module(module_name, repo_root, check=args.check)
        exit_code = max(exit_code, module_exit)

    return exit_code


if __name__ == "__main__":
    raise SystemExit(main())
