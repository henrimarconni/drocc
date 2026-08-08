use crate::ffi::c::{__BindgenBitfieldUnit, SMSpanInfo, SrcID};
use crate::ffi::{SourceManager, TokenStream};
use crate::view::View;
use crossterm::event::KeyCode;
use ratatui::prelude::*;
use ratatui::widgets::{Block, Borders, Paragraph};

pub struct LexerTab {
    active_panel: usize, // 0: Source, 1: Info
    stream: TokenStream,
    sman: SourceManager,
}

impl LexerTab {
    pub fn new(
        mut sman: SourceManager,
        srcid: SrcID,
        interner: &crate::ffi::StringInterner,
    ) -> Self {
        Self {
            active_panel: 0,
            stream: TokenStream::from_lexer(&mut sman, srcid, interner),
            sman,
        }
    }
}

impl View for LexerTab {
    fn title(&self) -> &'static str {
        "Lexer"
    }

    fn handle_input(&mut self, key: KeyCode) -> bool {
        match key {
            KeyCode::Tab => {
                self.active_panel = (self.active_panel + 1) % 2; // Toggle between 2 panels
                true
            }
            // You will add KeyCode::Down/Up here later to scroll the token list!
            _ => false,
        }
    }

    fn render(&self, f: &mut Frame, area: Rect) {
        let active_color = Color::White;
        let inactive_color = Color::DarkGray;
        let layout = Layout::default()
            .direction(Direction::Horizontal)
            .constraints([Constraint::Percentage(50), Constraint::Percentage(50)])
            .split(area);

        let c0 = if self.active_panel == 0 {
            active_color
        } else {
            inactive_color
        };
        let c1 = if self.active_panel == 1 {
            active_color
        } else {
            inactive_color
        };

        let source = Paragraph::new().block(
            Block::default()
                .borders(Borders::ALL)
                .title(" Source ")
                .fg(c0),
        );
        let info = Paragraph::new("Tokens will go here").block(
            Block::default()
                .borders(Borders::ALL)
                .title(" Info ")
                .fg(c1),
        );

        f.render_widget(source, layout[0]);
        f.render_widget(info, layout[1]);
    }
}
