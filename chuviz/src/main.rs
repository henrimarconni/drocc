use std::{error::Error, io};

use chuviz::{
    app::App,
    ffi::{Arena, SourceManager, StringInterner},
    lexer::LexerTab,
};
use crossterm::{
    event::{self, Event},
    execute,
    terminal::{EnterAlternateScreen, LeaveAlternateScreen, disable_raw_mode, enable_raw_mode},
};
use ratatui::{Terminal, prelude::CrosstermBackend};

fn main() -> Result<(), Box<dyn Error>> {
    let mut args = std::env::args();
    if args.len() != 2 {
        panic!("A single file path must be provided");
    }

    let mut sman = SourceManager::new();
    let arena = Arena::new(128 * 1024);
    let interner = StringInterner::new(&arena);
    let srcid = sman.open(args.nth(1).unwrap().as_str(), &arena)?;
    let lexertab = LexerTab::new(&mut sman, srcid, &interner);
    let mut app = App::new(Box::new(lexertab));

    enable_raw_mode()?;
    let mut stderr = io::stderr();
    execute!(stderr, EnterAlternateScreen)?;
    let mut terminal = Terminal::new(CrosstermBackend::new(stderr))?;

    loop {
        terminal.draw(|f| app.draw_ui(f))?;

        if let Event::Key(key) = event::read()? {
            app.input(key);
        }

        if app.should_quit {
            break;
        }
    }

    disable_raw_mode()?;
    execute!(terminal.backend_mut(), LeaveAlternateScreen)?;
    Ok(())
}
