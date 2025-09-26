#ifndef BUNDLES_H
#define BUNDLES_H

typedef struct {
    const char *bundle;
    const char *action;
    const char *test;     // NULL means no test (always runs)
    const char *command;
} BundleCommand;

static const BundleCommand bundle_commands[] = {
    // Rails bundle
    {"rails", "test", "test -f Gemfile", "rails test"},
    {"rails", "format", "test -f Gemfile", "bundle exec rubocop -A"},
    {"rails", "lint", "test -f Gemfile", "bundle exec rubocop"},
    {"rails", "run", "test -f Gemfile", "bin/dev"},
    {"rails", "console", "test -f Gemfile", "rails console"},
    {"rails", "migrate", "test -f Gemfile", "rails db:migrate"},

    // Node bundle
    {"node", "test", "test -f package.json", "npm test"},
    {"node", "build", "test -f package.json", "npm run build"},
    {"node", "run", "test -f package.json", "npm start"},
    {"node", "dev", "test -f package.json", "npm run dev"},
    {"node", "lint", "test -f package.json", "npm run lint"},
    {"node", "format", "test -f package.json", "npx prettier --write ."},
    {"node", "install", "test -f package.json", "npm install"},

    // Python bundle
    {"python", "test", "test -f requirements.txt", "python -m pytest"},
    {"python", "run", "test -f main.py", "python main.py"},
    {"python", "lint", "test -f requirements.txt", "python -m flake8"},
    {"python", "format", "test -f requirements.txt", "python -m black ."},
    {"python", "install", "test -f requirements.txt", "pip install -r requirements.txt"},

    // Go bundle
    {"go", "test", "test -f go.mod", "go test ./..."},
    {"go", "build", "test -f go.mod", "go build"},
    {"go", "run", "test -f main.go", "go run main.go"},
    {"go", "lint", "test -f go.mod", "golangci-lint run"},
    {"go", "format", "test -f go.mod", "go fmt ./..."},
    {"go", "install", "test -f go.mod", "go mod download"},

    // Rust bundle
    {"rust", "test", "test -f Cargo.toml", "cargo test"},
    {"rust", "build", "test -f Cargo.toml", "cargo build"},
    {"rust", "run", "test -f Cargo.toml", "cargo run"},
    {"rust", "lint", "test -f Cargo.toml", "cargo clippy"},
    {"rust", "format", "test -f Cargo.toml", "cargo fmt"},
    {"rust", "clean", "test -f Cargo.toml", "cargo clean"},

    // Sentinel
    {NULL, NULL, NULL, NULL}
};

// Helper function to get unique bundle names
static const char* get_bundle_names() {
    static char names[256] = {0};
    if (names[0] == 0) {
        char seen[10][32] = {0};
        int seen_count = 0;

        for (int i = 0; bundle_commands[i].bundle != NULL; i++) {
            int found = 0;
            for (int j = 0; j < seen_count; j++) {
                if (strcmp(seen[j], bundle_commands[i].bundle) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && seen_count < 10) {
                strcpy(seen[seen_count++], bundle_commands[i].bundle);
                if (names[0] != 0) strcat(names, ", ");
                strcat(names, bundle_commands[i].bundle);
            }
        }
    }
    return names;
}

#endif