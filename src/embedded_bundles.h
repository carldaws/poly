#ifndef EMBEDDED_BUNDLES_H
#define EMBEDDED_BUNDLES_H

typedef struct {
    const char *name;
    const char *content;
} Bundle;

static const Bundle embedded_bundles[] = {
    {"node", 
        "-- Node.js bundle\nreturn {\n    test = {\n        { test = \"test -f package.json\", command = \"npm test\" },\n    },\n    build = {\n        { test = \"test -f package.json\", command = \"npm run build\" },\n    },\n    run = {\n        { test = \"test -f package.json\", command = \"npm start\" },\n    },\n    dev = {\n        { test = \"test -f package.json\", command = \"npm run dev\" },\n    },\n    lint = {\n        { test = \"test -f package.json\", command = \"npm run lint\" },\n    },\n    format = {\n        { test = \"test -f package.json\", command = \"npx prettier --write .\" },\n    },\n    install = {\n        { test = \"test -f package.json\", command = \"npm install\" },\n    },\n}\n"},
    {"rails", 
        "-- Rails/Ruby bundle\nreturn {\n    test = {\n        { test = \"test -f Gemfile\", command = \"rails test\" },\n    },\n    format = {\n        { test = \"test -f Gemfile\", command = \"bundle exec rubocop -A\" },\n    },\n    lint = {\n        { test = \"test -f Gemfile\", command = \"bundle exec rubocop\" },\n    },\n    run = {\n        { test = \"test -f Gemfile\", command = \"bin/dev\" },\n    },\n    console = {\n        { test = \"test -f Gemfile\", command = \"rails console\" },\n    },\n    migrate = {\n        { test = \"test -f Gemfile\", command = \"rails db:migrate\" },\n    },\n}\n"},
    {NULL, NULL}
};

#endif
