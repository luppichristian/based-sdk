#!/usr/bin/env python3

import argparse
import json
import os
import subprocess
import sys
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Run all discovered *-tests executables once"
    )
    parser.add_argument("--build-dir", required=True, help="CMake build directory")
    parser.add_argument(
        "--build-config", required=True, help="CTest build configuration"
    )
    return parser.parse_args()


def discover_test_executables(build_dir: Path, build_config: str) -> list[str]:
    cmd = [
        "ctest",
        "--test-dir",
        str(build_dir),
        "--build-config",
        build_config,
        "--show-only=json-v1",
    ]
    print("Discovering tests via:", " ".join(cmd), flush=True)
    proc = subprocess.run(cmd, check=False, capture_output=True, text=True)
    if proc.returncode != 0:
        if proc.stdout:
            print(proc.stdout)
        if proc.stderr:
            print(proc.stderr, file=sys.stderr)
        raise RuntimeError(f"ctest discovery failed with exit code {proc.returncode}")

    data = json.loads(proc.stdout)
    tests = data.get("tests", [])

    executables: list[str] = []
    seen: set[str] = set()
    for test in tests:
        command = test.get("command", [])
        if not command:
            continue
        exe = command[0]
        name = Path(exe).name
        if not (name.endswith("-tests") or name.endswith("-tests.exe")):
            continue
        norm = os.path.normcase(os.path.normpath(exe))
        if norm in seen:
            continue
        seen.add(norm)
        executables.append(exe)

    return executables


def run_executables(executables: list[str]) -> int:
    if not executables:
        print(
            "No *-tests executables found from discovered CTest metadata",
            file=sys.stderr,
        )
        return 1

    print("Discovered test executables:", flush=True)
    for exe in executables:
        print(f"  - {exe}", flush=True)

    for exe in executables:
        print(f"\n=== Running {exe} ===", flush=True)
        proc = subprocess.run([exe], check=False)
        if proc.returncode != 0:
            print(
                f"Test executable failed: {exe} (exit={proc.returncode})",
                file=sys.stderr,
            )
            return proc.returncode

    return 0


def main() -> int:
    args = parse_args()
    build_dir = Path(args.build_dir)
    executables = discover_test_executables(build_dir, args.build_config)
    return run_executables(executables)


if __name__ == "__main__":
    sys.exit(main())
