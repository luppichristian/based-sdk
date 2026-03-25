#!/usr/bin/env python3

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path
import re
import sys


DEFAULT_MODULES = ("core", "gfx")

MODULE_CONFIG = {
    "core": {
        "title": "Core Module Quick Guide",
        "description": "`core` is the foundational module of based. Use it for platform abstractions, memory/context systems, containers, I/O and filesystem utilities, input/event flow, threading, and shared diagnostics/logging primitives.",
        "areas": [
            (
                "basic",
                "Compiler/platform layer, primitive types, assertions, crash routing, intrinsics, and foundational macros.",
            ),
            (
                "containers",
                "Intrusive containers and sorting helpers for lightweight data structures.",
            ),
            (
                "context",
                "Global/thread context setup, allocators, user data slots, and log-state access.",
            ),
            (
                "filesystem",
                "Paths, files, streams, archives, modules, and filesystem monitoring.",
            ),
            (
                "input",
                "Input devices, events, message payloads, and virtual-key helpers.",
            ),
            (
                "interface",
                "Windowing, monitor, cursor, icon, dialog, DPI, and text-input APIs.",
            ),
            (
                "memory",
                "Allocators, arenas, heaps, pools, rings, scratch buffers, and memory helpers.",
            ),
            ("processes", "Process launching, pipes, and current-process utilities."),
            ("strings", "Character, string, C-string, and Unicode conversion helpers."),
            ("system", "CPU, hardware, locale, and runtime/system information."),
            (
                "threads",
                "Threading, atomics, mutexes, semaphores, and synchronization primitives.",
            ),
            (
                "utils",
                "Utility helpers such as command line parsing, compression, logging, timers, IDs, and versioning.",
            ),
        ],
    },
    "gfx": {
        "title": "Gfx Module Quick Guide",
        "description": "`gfx` is the rendering-facing module built on top of `core`. It exposes module-level graphics entry points and serves as the integration surface for graphics backends.",
        "areas": [
            ("gfx", "Graphics module-facing API for availability and module identity."),
        ],
    },
}


@dataclass
class Symbol:
    name: str
    kind: str
    detail: str


@dataclass
class HeaderDoc:
    rel_path: str
    area: str
    types: list[Symbol]
    functions: list[Symbol]
    macros: list[Symbol]


DEFINE_RE = re.compile(
    r"^#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)(\((?:[^()]|\([^)]*\))*\))?"
)
FUNC_NAME_RE = re.compile(r"\b([A-Za-z_][A-Za-z0-9_]*)\s*\(")
FUNC_DECL_RE = re.compile(r"^func\s+(.+?)\s*;?$", re.DOTALL)
TYPEDEF_FUNC_PTR_RE = re.compile(r"\(\*\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)")
TYPEDEF_FUNC_RE = re.compile(
    r"typedef\s+.+?\b([A-Za-z_][A-Za-z0-9_]*)\s*\([^;]*\)\s*;", re.DOTALL
)
TYPEDEF_TAIL_RE = re.compile(
    r"\b([A-Za-z_][A-Za-z0-9_]*)\s*(?:\[[^\]]+\])?\s*;$",
    re.DOTALL,
)
WHITESPACE_RE = re.compile(r"\s+")


def _clean_space(text: str) -> str:
    return WHITESPACE_RE.sub(" ", text).strip()


def _iter_public_headers(module_dir: Path) -> list[Path]:
    include_dir = module_dir / "include"
    headers = []
    for header in sorted(include_dir.rglob("*.h")):
        rel_parts = header.relative_to(include_dir).parts
        if len(rel_parts) == 1 and rel_parts[0].startswith("based_"):
            continue
        headers.append(header)
    return headers


def _strip_block_comments(text: str) -> str:
    return re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)


def _macro_symbol(line: str) -> Symbol | None:
    match = DEFINE_RE.match(line)
    if not match:
        return None
    name = match.group(1)
    params = match.group(2) or ""
    detail = f"`{name}{params}`"
    return Symbol(name=name, kind="macro", detail=detail)


def _function_symbol(decl: str) -> Symbol | None:
    compact = _clean_space(decl)
    match = FUNC_DECL_RE.match(compact)
    if not match:
        return None
    signature = match.group(1)
    names = FUNC_NAME_RE.findall(signature)
    if not names:
        return None
    name = names[-1]
    return Symbol(name=name, kind="function", detail=f"`{compact}`")


def _typedef_symbol(decl: str) -> Symbol | None:
    compact = _clean_space(decl)
    name = ""
    kind = "alias"
    detail = f"`{compact}`"

    if compact.startswith("typedef struct "):
        kind = "struct"
    elif compact.startswith("typedef enum "):
        kind = "enum"
    elif compact.startswith("typedef union "):
        kind = "union"

    func_ptr_match = TYPEDEF_FUNC_PTR_RE.search(compact)
    if func_ptr_match:
        name = func_ptr_match.group(1)
        kind = "callback"
    else:
        func_match = TYPEDEF_FUNC_RE.match(compact)
        if func_match:
            name = func_match.group(1)
            kind = "callback"
        else:
            if "}" in compact:
                tail = compact.rsplit("}", 1)[1].strip()
                tail_match = re.match(r"([A-Za-z_][A-Za-z0-9_]*)\s*;", tail)
                if tail_match:
                    name = tail_match.group(1)
                    if kind in {"struct", "enum", "union"}:
                        detail = f"`typedef {kind} {name} {{ ... }} {name};`"

            tail_match = TYPEDEF_TAIL_RE.search(compact)
            if not name and tail_match:
                name = tail_match.group(1) or ""

    if not name:
        return None

    return Symbol(name=name, kind=kind, detail=detail)


def _parse_header(header_path: Path, include_dir: Path) -> HeaderDoc:
    rel_path = header_path.relative_to(include_dir).as_posix()
    area = rel_path.split("/", 1)[0]
    text = _strip_block_comments(header_path.read_text(encoding="utf-8"))
    lines = text.splitlines()

    types: list[Symbol] = []
    functions: list[Symbol] = []
    macros: list[Symbol] = []
    seen: set[tuple[str, str]] = set()

    idx = 0
    while idx < len(lines):
        stripped = lines[idx].strip()

        if not stripped or stripped.startswith("//") or stripped.startswith("#include"):
            idx += 1
            continue

        if stripped.startswith("#"):
            macro = _macro_symbol(stripped)
            if macro is not None and ("macro", macro.name) not in seen:
                seen.add(("macro", macro.name))
                macros.append(macro)

            while stripped.endswith("\\") and idx + 1 < len(lines):
                idx += 1
                stripped = lines[idx].rstrip()

            idx += 1
            continue

        if stripped.startswith("func "):
            decl_parts = [stripped]
            while not decl_parts[-1].rstrip().endswith(";") and idx + 1 < len(lines):
                idx += 1
                next_line = lines[idx].strip()
                if next_line.startswith("//"):
                    continue
                decl_parts.append(next_line)

            symbol = _function_symbol(" ".join(decl_parts))
            if symbol is not None and ("function", symbol.name) not in seen:
                seen.add(("function", symbol.name))
                functions.append(symbol)

            idx += 1
            continue

        if stripped.startswith("typedef "):
            decl_parts = [stripped]
            brace_depth = stripped.count("{") - stripped.count("}")
            while idx + 1 < len(lines):
                if brace_depth <= 0 and decl_parts[-1].rstrip().endswith(";"):
                    break
                idx += 1
                next_line = lines[idx].strip()
                if next_line.startswith("//"):
                    continue
                decl_parts.append(next_line)
                brace_depth += next_line.count("{") - next_line.count("}")

            symbol = _typedef_symbol(" ".join(decl_parts))
            if symbol is not None and ("type", symbol.name) not in seen:
                seen.add(("type", symbol.name))
                types.append(symbol)

            idx += 1
            continue

        idx += 1

    return HeaderDoc(
        rel_path=rel_path, area=area, types=types, functions=functions, macros=macros
    )


def _render_symbol_table(columns: tuple[str, str], symbols: list[Symbol]) -> list[str]:
    left, right = columns
    if not symbols:
        return [f"| {left} | {right} |", "| --- | --- |", f"| None | - |"]

    rows = [f"| {left} | {right} |", "| --- | --- |"]
    for symbol in symbols:
        rows.append(f"| `{symbol.name}` | {symbol.detail} |")
    return rows


def _render_module_doc(module_name: str, module_dir: Path) -> str:
    config = MODULE_CONFIG[module_name]
    include_dir = module_dir / "include"
    headers = [
        _parse_header(header, include_dir)
        for header in _iter_public_headers(module_dir)
    ]

    lines: list[str] = [
        f"# {config['title']}",
        "",
        config["description"],
        "",
        "The canonical API reference remains `./api_index.txt`. This guide is organized by API area and then by header so you can quickly locate public types, functions, and macros.",
        "",
        "## API Areas",
        "",
    ]

    for area_name, _ in config["areas"]:
        lines.append(f"- `{area_name}`")

    for area_name, area_desc in config["areas"]:
        lines.extend(["", f"## {area_name}", "", area_desc])
        area_headers = [header for header in headers if header.area == area_name]

        for header in area_headers:
            lines.extend(
                [
                    "",
                    f"### `{header.rel_path}`",
                    "",
                    f"- Types: **{len(header.types)}**",
                    f"- Functions: **{len(header.functions)}**",
                    f"- Macros: **{len(header.macros)}**",
                    "",
                    "#### Types",
                    "",
                    *_render_symbol_table(("Type", "Declaration"), header.types),
                    "",
                    "#### Functions",
                    "",
                    *_render_symbol_table(
                        ("Function", "Declaration"), header.functions
                    ),
                    "",
                    "#### Macros",
                    "",
                    *_render_symbol_table(("Macro", "Form"), header.macros),
                ]
            )

    return "\n".join(lines).rstrip() + "\n"


def _process_module(module_name: str, repo_root: Path, check: bool) -> int:
    if module_name not in MODULE_CONFIG:
        print(
            f"[error] Unsupported module for doc generation: {module_name}",
            file=sys.stderr,
        )
        return 1

    module_dir = repo_root / "modules" / module_name
    if not module_dir.is_dir():
        print(f"[error] Module directory does not exist: {module_dir}", file=sys.stderr)
        return 1

    doc_path = module_dir / "DOC.md"
    generated_content = _render_module_doc(module_name, module_dir)
    existing_content = (
        doc_path.read_text(encoding="utf-8") if doc_path.is_file() else ""
    )
    is_stale = existing_content != generated_content

    if check:
        if is_stale:
            print(f"[stale] {doc_path}")
            return 1
        print(f"[ok] {doc_path}")
        return 0

    if is_stale:
        doc_path.write_text(generated_content, encoding="utf-8")
        print(f"[regenerated] {doc_path}")
    else:
        print(f"[up-to-date] {doc_path}")
    return 0


def parse_args() -> argparse.Namespace:
    script_dir = Path(__file__).resolve().parent
    default_root = script_dir.parent

    parser = argparse.ArgumentParser(
        description="Generate module DOC.md files by parsing public headers"
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
        help="Check if docs are up to date without writing files",
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
