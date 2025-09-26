-- Node.js bundle
return {
    test = {
        { test = "test -f package.json", command = "npm test" },
    },
    build = {
        { test = "test -f package.json", command = "npm run build" },
    },
    run = {
        { test = "test -f package.json", command = "npm start" },
    },
    dev = {
        { test = "test -f package.json", command = "npm run dev" },
    },
    lint = {
        { test = "test -f package.json", command = "npm run lint" },
    },
    format = {
        { test = "test -f package.json", command = "npx prettier --write ." },
    },
    install = {
        { test = "test -f package.json", command = "npm install" },
    },
}