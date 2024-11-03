vim.g.vimrc_dap_lldb_vscode_path = vim.fn.expand("$LLDB_VS_CODE_PATH")
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
        require("vimrc.dap").init({
            language = "cpp",
            name = "cpplox",
            program = project_path .. "/build/lox_cli",
            cwd = project_path .. "/build",
            env = {},
            run_in_terminal = true,
        })
    end,
})

vim.keymap.set("n", "<leader>ab", ":silent Just build<CR>")
vim.keymap.set("n", "<leader>ac", ":silent Just conf<CR>")
