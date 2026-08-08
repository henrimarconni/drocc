use ratatui::{
    Frame,
    layout::{Constraint, Direction, Layout},
    style::{Color, Modifier, Style},
    text::Line,
    widgets::{Block, Borders, Tabs},
};

use super::view::View;
use crossterm::event::{KeyCode, KeyEvent, KeyModifiers};

pub struct App {
    pub tabs: Vec<Box<dyn View>>,
    pub current_tab: usize,
    pub should_quit: bool,
}

impl App {
    pub fn new(deftab: Box<dyn View>) -> Self {
        Self {
            tabs: vec![deftab],
            current_tab: 0,
            should_quit: false,
        }
    }

    fn active_tab_mut(&mut self) -> &mut Box<dyn View> {
        &mut self.tabs[self.current_tab]
    }

    pub fn input(&mut self, key: KeyEvent) {
        let handled_by_tab = self.active_tab_mut().handle_input(key.code);
        if !handled_by_tab {
            match (key.modifiers, key.code) {
                (KeyModifiers::NONE, KeyCode::Char('q')) => self.should_quit = true,
                (KeyModifiers::ALT, KeyCode::Char('1')) => self.current_tab = 0,
                (KeyModifiers::ALT, KeyCode::Char('2')) => self.current_tab = 1,
                (KeyModifiers::SHIFT, KeyCode::Right) => {
                    self.current_tab = (self.current_tab + 1) % self.tabs.len();
                }
                (KeyModifiers::SHIFT, KeyCode::Left) => {
                    if self.current_tab == 0 {
                        self.current_tab = self.tabs.len() - 1;
                    } else {
                        self.current_tab -= 1;
                    }
                }
                _ => {}
            }
        }
        self.current_tab %= self.tabs.len();
    }

    pub fn draw_ui(&self, f: &mut Frame) {
        let layout = Layout::default()
            .direction(Direction::Vertical)
            .constraints([Constraint::Length(3), Constraint::Min(0)])
            .split(f.area());

        let tab_titles: Vec<Line> = self.tabs.iter().map(|t| Line::from(t.title())).collect();

        let tabs = Tabs::new(tab_titles)
            .block(
                Block::default()
                    .borders(Borders::ALL)
                    .title(" chuviz: chucci visualizer "),
            )
            .select(self.current_tab)
            .style(Style::default().fg(Color::Cyan))
            .highlight_style(
                Style::default()
                    .fg(Color::Yellow)
                    .add_modifier(Modifier::BOLD),
            );

        f.render_widget(tabs, layout[0]);

        self.tabs[self.current_tab].render(f, layout[1]);
    }
}
