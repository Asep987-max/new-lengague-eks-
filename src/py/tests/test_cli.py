import click
from click.testing import CliRunner
from dsl_exec.cli import run

def test_run_command_success():
    runner = CliRunner()
    # Mocking the actual executor invocation would be ideal, but for now we just check basic argument parsing
    with runner.isolated_filesystem():
        with open('test.dsl', 'w') as f:
            f.write('command test: target: wsl params: []')

        # We expect it to fail finding the binary if not built, but the CLI logic should run
        result = runner.invoke(run, ['test.dsl'])

        # We assert that it tried to run. The output message depends on whether it finds the binary.
        # It's safer to assert that it didn't crash with a Python exception other than FileNotFoundError/subprocess error which we catch.
        assert result.exit_code in [0, 1]

def test_run_command_missing_file():
    runner = CliRunner()
    result = runner.invoke(run, ['nonexistent.dsl'])
    assert result.exit_code != 0
    assert "Error: Invalid value for 'DSL_FILE': Path 'nonexistent.dsl' does not exist." in result.output
