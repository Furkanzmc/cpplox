vim.g.vimrc_dap_lldb_vscode_path = "/usr/local/bin/lldb-vscode"
local project_path = vim.fn.expand("$CPPLOX_PATH")

vim.api.nvim_create_user_command("DebugTest", function(command)
    local opts = {
        name = command.args,
        env = {},
    }
    require("dap").run({
        type = "cpp",
        request = "launch",
        name = opts.name,
        program = project_path .. "/build/test/" .. opts.name,
        symbolSearchPath = project_path .. "/build/",
        cwd = project_path .. "/build/test",
        debuggerRoot = project_path .. "/build/test",
        env = opts.env,
        runInTerminal = true,
    })
end, {
    nargs = 1,
    complete = function()
        return { "test_parser", "test_scanner", "test_interpreter" }
    end,
})

local augroup_nvimrc = vim.api.nvim_create_augroup("nvimrc", { clear = true })

vim.api.nvim_create_autocmd({ "VimEnter" }, {
    pattern = "*",
    group = augroup_nvimrc,
    callback = function(_)
        require("vimrc.cpp").setup_cmake({
            cwd = project_path .. "/build",
            env = {},
            name = "cpplox",
            program = project_path .. "/build/lox_cli",
            build_dir = project_path .. "/build",
            project_path = project_path,
            test_cwd = project_path .. "/build/test",
            generator = "Ninja",
            cmake_args = {},
        })
    end,
})

vim.cmd([[cabbrev build Build]])
vim.cmd([[cabbrev run Run]])
vim.cmd([[cabbrev test RunTests]])
vim.cmd([[cabbrev dtest DebugTest]])
vim.cmd([[cabbrev cmake CMake]])

vim.cmd([[nmap <leader>ab :silent Build<CR>]])
vim.cmd([[nmap <leader>ar :silent Run<CR>]])
vim.cmd([[nmap <leader>at :silent RunTests<CR>]])
vim.cmd([[nmap <leader>ac :silent CMake<CR>]])
