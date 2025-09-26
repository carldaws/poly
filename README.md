# poly

A universal command runner that provides consistent commands across different project types. Instead of remembering `npm test`, `cargo test`, `go test`, or `rails test`, just use `poly test`.

## Features

- **Single binary** - No dependencies, includes embedded Lua interpreter
- **Auto-detection** - Automatically detects project type and runs appropriate commands
- **Extensible** - Configure custom commands via `poly.lua` files (local or global)
- **Built-in bundles** - Includes commands for Rails, Node.js, Python, Go, and Rust projects
- **Multiple commands** - Can run multiple matching commands (e.g., format with both Prettier and RuboCop)

## Installation

### From Source

```bash
git clone https://github.com/carldaws/poly.git
cd poly
make
sudo make install
```

## How It Works

Poly is a command runner that reads from configuration files. There are no built-in commands like `test` or `build` - instead, YOU define what these commands do for your projects.

When you run `poly <command>`, poly:
1. Looks for the command in local `poly.lua` or global `~/poly.lua`
2. For each matching command entry, checks if there's a test condition
3. Runs the test (if present) to determine if the command applies
4. Executes all matching commands

## Configuration

Poly uses Lua configuration files to define commands:

- `~/poly.lua` - Global configuration (with test conditions)
- `./poly.lua` - Local project configuration (overrides global)

### Example Configuration

```lua
-- poly.lua
return {
    test = {
        { command = "npm test" },
    },
    build = {
        { command = "npm run build" },
    },
    start = {
        { command = "npm start" },
    },
}
```

### With Test Conditions (Global Config)

```lua
-- ~/poly.lua
return {
    test = {
        { test = "test -f package.json", command = "npm test" },
        { test = "test -f Gemfile", command = "rails test" },
        { test = "test -f go.mod", command = "go test ./..." },
    },
    format = {
        { test = "test -f package.json", command = "prettier --write ." },
        { test = "test -f Gemfile", command = "rubocop -A" },
    },
}
```

The `test` field can be **any shell command** that returns 0 for success:
- `test -f Gemfile` - Check if a file exists
- `test -d .git` - Check if a directory exists
- `grep -q '"react"' package.json` - Check if package.json contains "react"
- `which docker` - Check if docker is installed
- `[ -f .env.local ] && grep -q RAILS_ENV .env.local` - Complex conditions

When multiple test conditions match, **all matching commands are executed**.

## Quick Start

### 1. Add Commands for Your Project Type

```bash
# Add bundle commands to local poly.lua (for this project only)
poly add rails
poly add node

# Or add to global ~/poly.lua (for all projects)
poly add rails --global
poly add node --global
```

### 2. Use Your Commands

Once configured, use the same commands everywhere:

```bash
poly test       # Runs appropriate test command(s)
poly build      # Builds your project
poly run        # Starts your application
poly format     # Formats code
poly lint       # Lints code
```

### 3. See What's Available

```bash
poly list       # Shows which commands will run in current directory
poly help       # Shows usage information
```

## Command Bundles

Poly includes pre-defined command bundles that you can add to your configuration. These define common commands for different project types:

### Rails/Ruby

- `test` - rails test
- `format` - bundle exec rubocop -A
- `lint` - bundle exec rubocop
- `run` - bin/dev
- `console` - rails console
- `migrate` - rails db:migrate

### Node.js

- `test` - npm test
- `build` - npm run build
- `run` - npm start
- `dev` - npm run dev
- `lint` - npm run lint
- `format` - npx prettier --write .
- `install` - npm install

### Python

- `test` - python -m pytest
- `run` - python main.py
- `lint` - python -m flake8
- `format` - python -m black .
- `install` - pip install -r requirements.txt

### Go

- `test` - go test ./...
- `build` - go build
- `run` - go run main.go
- `lint` - golangci-lint run
- `format` - go fmt ./...
- `install` - go mod download

### Rust

- `test` - cargo test
- `build` - cargo build
- `run` - cargo run
- `lint` - cargo clippy
- `format` - cargo fmt
- `clean` - cargo clean


## Building from Source

Requirements:

- C compiler (gcc or clang)
- make

The build process automatically downloads and embeds Lua 5.4, so no external dependencies are required.

```bash
make        # Build poly
make clean  # Clean build files
make install   # Install to /usr/local/bin
make uninstall # Remove from /usr/local/bin
```

## License

MIT

## Contributing

Pull requests welcome! To add new default bundles, edit `src/bundles.h` and rebuild.

