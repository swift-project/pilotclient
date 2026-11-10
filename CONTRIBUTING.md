# Contributing

## Mini-GUI Architecture Guidelines

### Shared GUI components (src/gui/components, src/gui/editors)

Components in `src/gui/components` and `src/gui/editors` (e.g. `CLoginComponent`, `COwnAircraftComponent`,
`CSettingsNetworkServersComponent`) are shared between `swiftguistandard` and `swiftguimini`. These classes
**must not be modified** to accommodate Mini-GUI-specific UI requirements, since changes would affect
`swiftguistandard` as well.

### Mini-GUI specific customization pattern

When the Mini-GUI (`src/swiftguimini`) needs a different look/behavior than the shared component provides:

1. Create a new Mini-GUI-only wrapper class (plain C++, no `.ui` file), e.g. `CMiniLoginWidget`,
   `CMiniLoginSettingsDialog`.
2. Reuse the shared component instance, but access its internal named widgets via
   `findChild<T*>(QStringLiteral("objectName"))`, using the object names defined in the shared component's
   Designer `.ui` file.
3. Reparent/move widgets (e.g. `QGroupBox` sections) into new layouts/dialogs as needed, instead of editing
   the shared `.ui` file.
4. Keep new Mini-GUI-only classes in the **global namespace** (not inside `swift::gui::components`), consistent
   with existing classes like `CMiniLoginWidget`, `CMiniMiscSettingsDialog`. This avoids Qt moc generating
   namespace-qualified translation contexts (e.g. `swift::gui::components::CMiniCockpitWidget`) that must be
   matched exactly in `.ts` translation files.