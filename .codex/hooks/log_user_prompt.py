#!/usr/bin/env python3
import datetime
import json
import os
import pathlib
import re
import subprocess
import sys


def safe_filename(value: str) -> str:
    value = value.strip()
    if not value:
        return "unknown-session"
    return re.sub(r"[^A-Za-z0-9._-]+", "-", value).strip(".-") or "unknown-session"


def markdown_escape(text: str) -> str:
    return text.replace("\r\n", "\n").replace("\r", "\n")


def repo_root(codex_dir: pathlib.Path) -> pathlib.Path:
    try:
        out = subprocess.run(
            ["git", "rev-parse", "--show-toplevel"],
            cwd=codex_dir,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            check=False,
        )
        if out.returncode == 0 and out.stdout.strip():
            return pathlib.Path(out.stdout.strip()).resolve()
    except OSError:
        pass
    return codex_dir.parent.resolve()


def relativize_repo_paths(text: str, root: pathlib.Path) -> str:
    root_text = root.as_posix().rstrip("/")
    if not root_text:
        return text

    def replace(match: re.Match[str]) -> str:
        path_text = match.group(0)
        suffix = path_text[len(root_text):]
        if suffix == "":
            return "."
        if suffix.startswith("/"):
            return suffix[1:] or "."
        return path_text

    pattern = re.compile(re.escape(root_text) + r"(?:/[^\s`'\"<>)]*)?")
    return pattern.sub(replace, text)


def main() -> int:
    raw = sys.stdin.read()
    try:
        event = json.loads(raw) if raw.strip() else {}
    except json.JSONDecodeError as exc:
        event = {"parse_error": str(exc), "raw": raw}

    hook_name = (
        event.get("hook_event_name")
        or event.get("hookEventName")
        or event.get("type")
        or "UserPromptSubmit"
    )
    if hook_name != "UserPromptSubmit":
        return 0

    script_path = pathlib.Path(__file__).resolve()
    codex_dir = script_path.parent.parent
    root = repo_root(codex_dir)
    session_id = event.get("session_id") or event.get("sessionId") or "unknown-session"
    if "CODEX_HOOK_LOG_PATH" in os.environ:
        log_path = pathlib.Path(os.environ["CODEX_HOOK_LOG_PATH"])
    else:
        log_dir = pathlib.Path(os.environ.get("CODEX_HOOK_LOG_DIR", codex_dir / "logs"))
        log_path = log_dir / (safe_filename(str(session_id)) + ".md")
    log_path.parent.mkdir(parents=True, exist_ok=True)

    ts = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    model = str(event.get("model") or "")
    prompt = relativize_repo_paths(markdown_escape(str(event.get("prompt", ""))), root)
    needs_header = not log_path.exists() or log_path.stat().st_size == 0

    with log_path.open("a", encoding="utf-8") as fh:
        if needs_header:
            fh.write(f"# Codex Chat {session_id}\n\n")
        fh.write(f"## [{ts}][{model}]\n\n")
        fh.write(prompt)
        if prompt and not prompt.endswith("\n"):
            fh.write("\n")
        fh.write("\n")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
