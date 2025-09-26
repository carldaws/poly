#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define MAX_COMMAND_LENGTH 4096
#define GLOBAL_CONFIG_FILE "poly.lua"
#define LOCAL_CONFIG_FILE "poly.lua"

static lua_State *L = NULL;

char* find_global_config() {
    static char config_path[PATH_MAX];
    char *home = getenv("HOME");

    if (home) {
        snprintf(config_path, sizeof(config_path), "%s/%s", home, GLOBAL_CONFIG_FILE);
        if (access(config_path, F_OK) == 0) {
            return config_path;
        }
    }

    return NULL;
}

int load_config_file(const char *path) {
    if (luaL_dofile(L, path) != LUA_OK) {
        fprintf(stderr, "Error loading config from %s: %s\n", path, lua_tostring(L, -1));
        lua_pop(L, 1);
        return 0;
    }
    return 1;
}

void merge_configs() {
    lua_newtable(L);
    lua_setglobal(L, "merged_config");

    // Load global config first
    char *global_config = find_global_config();
    if (global_config) {
        if (load_config_file(global_config)) {
            // Store global config in merged_config
            lua_getglobal(L, "merged_config");
            int merged_idx = lua_gettop(L);

            // Iterate through returned table and copy to merged_config
            int config_idx = lua_gettop(L) - 1;
            lua_pushnil(L);
            while (lua_next(L, config_idx) != 0) {
                // key at -2, value at -1
                lua_pushvalue(L, -2);  // duplicate key
                lua_pushvalue(L, -2);  // duplicate value
                lua_settable(L, merged_idx);
                lua_pop(L, 1);  // remove value, keep key for next iteration
            }
            lua_pop(L, 2);  // pop config table and merged_config
        }
    }

    // Load local config and override
    if (access(LOCAL_CONFIG_FILE, F_OK) == 0) {
        if (load_config_file(LOCAL_CONFIG_FILE)) {
            lua_getglobal(L, "merged_config");
            int merged_idx = lua_gettop(L);

            // Iterate through returned table and override in merged_config
            int config_idx = lua_gettop(L) - 1;
            lua_pushnil(L);
            while (lua_next(L, config_idx) != 0) {
                // key at -2, value at -1
                lua_pushvalue(L, -2);  // duplicate key
                lua_pushvalue(L, -2);  // duplicate value
                lua_settable(L, merged_idx);
                lua_pop(L, 1);  // remove value, keep key for next iteration
            }
            lua_pop(L, 2);  // pop config table and merged_config
        }
    }
}

void execute_commands_for_action(const char *action, int argc, char *argv[]) {
    lua_getglobal(L, "merged_config");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        fprintf(stderr, "Error: No command found for '%s'\n", action);
        fprintf(stderr, "Make sure it's defined in your poly.lua config\n");
        return;
    }

    lua_pushstring(L, action);
    lua_gettable(L, -2);

    if (!lua_istable(L, -1)) {
        lua_pop(L, 2);
        fprintf(stderr, "Error: No command found for '%s'\n", action);
        fprintf(stderr, "Make sure it's defined in your poly.lua config\n");
        return;
    }

    int commands_executed = 0;

    // Iterate through the command list
    int len = lua_rawlen(L, -1);
    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, -1, i);

        if (lua_istable(L, -1)) {
            // Check if there's a test field
            lua_pushstring(L, "test");
            lua_gettable(L, -2);

            int should_run = 1;
            if (lua_isstring(L, -1)) {
                // Run the test command
                const char *test_cmd = lua_tostring(L, -1);
                int test_result = system(test_cmd);
                should_run = (test_result == 0);
            }
            lua_pop(L, 1);  // pop test value

            if (should_run) {
                // Get the command
                lua_pushstring(L, "command");
                lua_gettable(L, -2);

                if (lua_isstring(L, -1)) {
                    const char *command = lua_tostring(L, -1);

                    char full_command[MAX_COMMAND_LENGTH];
                    strcpy(full_command, command);

                    // Append any additional arguments
                    for (int j = 0; j < argc; j++) {
                        strcat(full_command, " ");
                        strcat(full_command, argv[j]);
                    }

                    printf("Executing: %s\n", full_command);

                    pid_t pid = fork();
                    if (pid == 0) {
                        // Child process
                        execl("/bin/sh", "sh", "-c", full_command, NULL);
                        perror("execl failed");
                        exit(1);
                    } else if (pid > 0) {
                        // Parent process
                        int status;
                        waitpid(pid, &status, 0);
                        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                            fprintf(stderr, "Command failed with exit code %d\n", WEXITSTATUS(status));
                        }
                        commands_executed++;
                    } else {
                        perror("fork failed");
                    }
                }
                lua_pop(L, 1);  // pop command value
            }
        }
        lua_pop(L, 1);  // pop table entry
    }

    lua_pop(L, 2);  // pop action table and merged_config

    if (commands_executed == 0) {
        fprintf(stderr, "Error: No matching command found for '%s'\n", action);
        fprintf(stderr, "None of the test conditions matched\n");
    }
}


void print_usage() {
    printf("Usage: poly <command> [args...]\n");
    printf("\nBuilt-in commands:\n");
    printf("  list     Show all available commands\n");
    printf("  help     Show this help message\n");
    printf("\nCommands are defined in:\n");
    printf("  ~/poly.lua (global config)\n");
    printf("  ./poly.lua (local config - overrides global)\n");
    printf("\nExample config:\n");
    printf("  return {\n");
    printf("    build = {\n");
    printf("      { test = \"test -f Makefile\", command = \"make\" },\n");
    printf("      { command = \"echo 'No build system found'\" },\n");
    printf("    },\n");
    printf("    test = {\n");
    printf("      { test = \"test -f package.json\", command = \"npm test\" },\n");
    printf("    },\n");
    printf("  }\n");
}

void print_available_commands() {
    printf("Available commands:\n\n");
    printf("Built-in:\n");
    printf("  list     Show all available commands\n");
    printf("  help     Show usage information\n");

    lua_getglobal(L, "merged_config");
    if (lua_istable(L, -1)) {
        printf("\nConfigured (commands that will run in this directory):\n");

        // Iterate through the merged config table
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            // key at -2, value at -1
            if (lua_isstring(L, -2) && lua_istable(L, -1)) {
                const char *action_name = lua_tostring(L, -2);
                int matching_commands = 0;
                int first_matching = 1;

                // Count and display matching commands
                int len = lua_rawlen(L, -1);
                for (int i = 1; i <= len; i++) {
                    lua_rawgeti(L, -1, i);
                    if (lua_istable(L, -1)) {
                        // Check if there's a test field
                        lua_pushstring(L, "test");
                        lua_gettable(L, -2);

                        int will_run = 1;
                        if (lua_isstring(L, -1)) {
                            const char *test_cmd = lua_tostring(L, -1);
                            int test_result = system(test_cmd);
                            will_run = (test_result == 0);
                        }
                        lua_pop(L, 1);  // pop test value

                        if (will_run) {
                            if (first_matching) {
                                printf("  %-10s", action_name);
                                first_matching = 0;
                            } else {
                                printf("             ");
                            }

                            lua_pushstring(L, "command");
                            lua_gettable(L, -2);
                            if (lua_isstring(L, -1)) {
                                const char *cmd = lua_tostring(L, -1);
                                if (matching_commands > 0) {
                                    printf("+ ");
                                } else {
                                    printf("→ ");
                                }
                                if (strlen(cmd) > 65) {
                                    printf("%.62s...\n", cmd);
                                } else {
                                    printf("%s\n", cmd);
                                }
                                matching_commands++;
                            }
                            lua_pop(L, 1);  // pop command
                        }
                    }
                    lua_pop(L, 1);  // pop table entry
                }

                if (matching_commands > 1) {
                    printf("             (will run %d commands)\n", matching_commands);
                }
            }
            lua_pop(L, 1);  // remove value, keep key for next iteration
        }
    }
    lua_pop(L, 1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    // Check for built-in commands first (these take precedence)
    if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage();
        return 0;
    }

    // Initialize Lua
    L = luaL_newstate();
    luaL_openlibs(L);

    // Load and merge configurations
    merge_configs();

    // Check for list command after loading config so we can show what's available
    if (strcmp(argv[1], "list") == 0) {
        print_available_commands();
        lua_close(L);
        return 0;
    }

    // Execute all matching commands for the action
    execute_commands_for_action(argv[1], argc - 2, argv + 2);

    lua_close(L);
    return 0;
}