pub fn command<S: AsRef<str> + std::fmt::Debug>(name: S) -> &'static Command {
    let name = name.as_ref();
    for command in COMMANDS {
        if command.name == name {
            return command;
        }
    }
    panic!("unknown command: {:?}", name);
}

pub struct Command {
    pub name: &'static str,
    pub monitor_arg_index: usize,
    pub args: &'static [&'static str],
}

static COMMANDS: &[&Command] = &[
    &MENU_COMMAND,
    &FIREFOX_COMMAND,
    #[cfg(feature = "term")]
    &TERM_COMMAND,
    #[cfg(feature = "locker")]
    &LOCK_COMMAND,
];

static MENU_COMMAND: Command = Command {
    name: "menu",
    monitor_arg_index: 6,
    args: &[
        "rofi",
        "-modi",
        "drun",
        "-show",
        "drun",
        "-monitor",
        "-1", // monitor arg
        "-show-icons",
    ],
};

static FIREFOX_COMMAND: Command = Command {
    name: "firefox",
    monitor_arg_index: 0,
    args: &["firefox"],
};

#[cfg(feature = "term")]
static TERM_COMMAND: Command = Command {
    name: "term",
    monitor_arg_index: 0,
    #[cfg(feature = "term-alacritty")]
    args: &["alacritty"],
    #[cfg(feature = "term-gnome")]
    args: &["gnome-terminal", "--wait"],
    #[cfg(feature = "term-st")]
    args: &["st"],
    #[cfg(feature = "term-xterm")]
    args: &["xterm"],
};

#[cfg(feature = "locker")]
static LOCK_COMMAND: Command = Command {
    name: "lock",
    monitor_arg_index: 0,
    #[cfg(feature = "locker-i3lock")]
    args: &["i3lock"],
    #[cfg(feature = "locker-i3lock-color")]
    args: &[
        "i3lock",
        "--insidever-color=#ffffff22",
        "--ringver-color=#bb00bbbb",
        //
        "--insidewrong-color=#ffffff22",
        "--ringwrong-color=#880000bb",
        //
        "--inside-color=#00000000",
        "--ring-color=#ff00ffcc",
        "--line-color=#00000000",
        "--separator-color=#ff00ffcc",
        //
        "--verif-color=#ee00eeee",
        "--wrong-color=#ee00eeee",
        "--time-color=#ee00eeee",
        "--date-color=#ee00eeee",
        "--layout-color=#ee00eeee",
        "--keyhl-color=#880000bb",
        "--bshl-color=#880000bb",
        //
        "--screen=1",
        "--blur=5",
        "--clock",
        "--indicator",
        "--time-str=%H:%M:%S",
        "--date-str=%a, %e %b %Y",
        "--keylayout=1",
    ],
};
