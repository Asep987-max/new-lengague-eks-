command list_dir:
    target: wsl
    params: [path: string = "/home/user", max_entries: int = 100]
    timeout: 5000
    mode: safe
    description: "List directory using WSL helper"

command run_ps:
    target: powershell
    params: [script: string = "Get-Process"]
    timeout: 30000
    mode: safe
