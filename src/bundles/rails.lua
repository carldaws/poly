-- Rails/Ruby bundle
return {
    test = {
        { test = "test -f Gemfile", command = "rails test" },
    },
    format = {
        { test = "test -f Gemfile", command = "bundle exec rubocop -A" },
    },
    lint = {
        { test = "test -f Gemfile", command = "bundle exec rubocop" },
    },
    run = {
        { test = "test -f Gemfile", command = "bin/dev" },
    },
    console = {
        { test = "test -f Gemfile", command = "rails console" },
    },
    migrate = {
        { test = "test -f Gemfile", command = "rails db:migrate" },
    },
}