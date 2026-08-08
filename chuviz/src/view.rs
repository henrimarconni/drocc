use crossterm::event::KeyCode;
use ratatui::prelude::*;

pub trait View {
    /// The name displayed in the top Tab bar
    fn title(&self) -> &'static str;

    /// Handle keypresses specific to this tab.
    /// Returns `true` if the tab consumed the key (e.g., scrolled a list),
    /// or `false` if the App should handle it (e.g., switched tabs).
    fn handle_input(&mut self, key: KeyCode) -> bool;

    /// Draw this specific tab's UI
    fn render(&self, f: &mut Frame, area: Rect);
}
