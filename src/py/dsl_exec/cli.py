import click
import subprocess
import os
import sys

def find_executor_core():
    # Attempt to find relative to package install or build dir
    possible_paths = [
        "build/executor_core",
        "build/Release/executor_core.exe",
        "./executor_core",
        "../build/executor_core"  # If run from src/py
    ]
    for path in possible_paths:
        if os.path.exists(path):
            return os.path.abspath(path)
    # If not found, assume it's in PATH or just return name to fail nicely
    return "executor_core"

@click.group()
def main():
    """Executor DSL CLI"""
    pass

@main.command()
@click.argument('dsl_file', type=click.Path(exists=True))
@click.option('--allow-raw', is_flag=True, help="Allow raw command execution")
def run(dsl_file, allow_raw):
    """Run a command from a DSL file."""

    executor_path = find_executor_core()

    # Simple check if binary exists or if we should just try running it
    if not os.path.exists(executor_path) and executor_path != "executor_core":
        click.echo(f"Error: Executor core binary not found at {executor_path}", err=True)
        sys.exit(1)

    try:
        click.echo(f"Executing: {dsl_file} using {executor_path}")

        cmd = [executor_path, dsl_file]
        if allow_raw:
            cmd.append("--allow-raw")

        result = subprocess.run(cmd, capture_output=True, text=True)

        if result.returncode != 0:
            click.echo(f"Execution failed with code {result.returncode}", err=True)
            click.echo(result.stderr, err=True)
            sys.exit(result.returncode)
        else:
            click.echo(result.stdout)

    except FileNotFoundError:
        click.echo(f"Error: Executor core binary not found: {executor_path}", err=True)
        sys.exit(1)
    except Exception as e:
        click.echo(f"Error: {e}", err=True)
        sys.exit(1)

if __name__ == '__main__':
    main()
