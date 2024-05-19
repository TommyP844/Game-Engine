
function getLibFiles(directory)
    local libs = {}
    local p = io.popen('dir "'..directory..'" /b /a:-d')
    for file in p:lines() do
        if file:match("%.lib$") then
            table.insert(libs, directory .. "/" .. file)
        end
    end
    return libs
end