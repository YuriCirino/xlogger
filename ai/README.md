# xLogger for AI assistants

AI models don't know xLogger and tend to fall back to `Serial.print` and `String`.
This folder has a reference written for them. CI compiles every code example in it.

- **Any chat AI (ChatGPT, Claude, Gemini…):** attach [`llms-full.txt`](llms-full.txt) to your
  request, or paste its [raw URL](https://raw.githubusercontent.com/YuriCirino/xlogger/main/ai/llms-full.txt)
  if your AI can browse.
- **Coding agents (Claude Code, Copilot, Cursor, Codex, Gemini CLI…):** copy
  [`skills/xlogger`](skills/xlogger) into your tool's skills folder, e.g.
  `.claude/skills/xlogger/` for Claude Code. See [agentskills.io](https://agentskills.io/)
  for other tools.
- **PlatformIO projects:** the file comes with the library, at
  `.pio/libdeps/<env>/xLogger/ai/llms-full.txt`.

---

🇧🇷 **Português:** anexe o [`llms-full.txt`](llms-full.txt) no pedido para a IA, ou copie a
pasta [`skills/xlogger`](skills/xlogger) para a pasta de skills da sua ferramenta
(no Claude Code, `.claude/skills/xlogger/`).

`llms-full.txt` is generated from `skills/xlogger/SKILL.md`; edit the skill and run
`python3 extras/tools/ai_docs.py build`.
