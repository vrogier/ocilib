#!/usr/bin/env python3
"""
OCILIB tutorials static site builder.

Usage:
    python build.py            # build all pages into html/
    python build.py --check    # build + report internal links that resolve nowhere

Pages:  pages/NNN-slug.md  (NNN = ordering key, slug = output file name)
Output: html/<slug>.html   + copied assets + generated pygments css

Markdown conventions (see pages/ for examples):
    ---
    title: Page title          # required
    navtitle: Short name       # optional — sidebar/prev-next label (concept names only, no OCI_*)
    part: 0                    # part key from toc.yml
    level: beginner            # beginner | intermediate | advanced
    oracle: 12.2               # optional minimum Oracle version badge
    demos: [conn.c, conn.cpp]  # optional demo cross-references
    ---

    :::sample                  # tabbed code group; each fence = one tab
    ```c
    ...
    ```
    ```cpp
    ...
    ```
    :::

    !!! note|tip|warning|demo|oracle "optional title"   # callout box
"""

import html as html_mod
import re
import shutil
import sys
from pathlib import Path

try:
    import markdown
except ImportError:
    sys.exit("Missing dependency — run: pip install -r requirements.txt")

try:
    from pygments import highlight as pyg_highlight
    from pygments.formatters import HtmlFormatter
    from pygments.lexers import get_lexer_by_name, TextLexer
    from pygments.util import ClassNotFound

    HAS_PYGMENTS = True
except ImportError:
    HAS_PYGMENTS = False

ROOT = Path(__file__).resolve().parent
PAGES_DIR = ROOT / "pages"
ASSETS_DIR = ROOT / "assets"
OUT_DIR = ROOT / "html"

SITE_TITLE = "OCILIB Tutorials"
SITE_VERSION = "v5.0.0"

LANG_LABELS = {
    "c": "C", "cpp": "C++", "h": "C header", "sql": "SQL", "plsql": "PL/SQL",
    "sh": "Shell", "bash": "Shell", "powershell": "PowerShell", "cmd": "cmd",
    "cmake": "CMake", "make": "Makefile", "text": "Text", "output": "Output",
    "xml": "XML", "json": "JSON", "yaml": "YAML",
}

PYG_LANGS = {"h": "c", "plsql": "sql", "cmd": "bat", "sh": "bash", "make": "make", "output": "text"}

LEVEL_LABELS = {"beginner": "Beginner", "intermediate": "Intermediate", "advanced": "Advanced"}

FRONT_RE = re.compile(r"\A---\s*\n(.*?)\n---\s*\n", re.S)
SAMPLE_RE = re.compile(r"(?ms)^:::sample[^\n]*\n(.*?)^:::[ \t]*$")
FENCE_RE = re.compile(r"```([A-Za-z0-9_+-]*)[ \t]*\n(.*?)```", re.S)
TOKEN_FMT = "TUTORIALSAMPLETOKEN{}TOKEN"


# ---------------------------------------------------------------- front matter

def parse_front(text):
    """Minimal YAML-subset front matter: 'key: value' or 'key: [a, b]' lines."""
    meta, body = {}, text
    m = FRONT_RE.match(text)
    if m:
        for line in m.group(1).splitlines():
            lm = re.match(r"^([A-Za-z_]+)\s*:\s*(.*)$", line.strip())
            if lm:
                key, val = lm.group(1), lm.group(2).strip()
                if val.startswith("[") and val.endswith("]"):
                    meta[key] = [v.strip() for v in val[1:-1].split(",") if v.strip()]
                else:
                    meta[key] = val.strip('"').strip("'")
        body = text[m.end():]
    return meta, body


def parse_parts():
    """Read 'key: Name' lines under 'parts:' in toc.yml (minimal parser)."""
    parts = {}
    toc = ROOT / "toc.yml"
    in_parts = False
    for line in toc.read_text(encoding="utf-8").splitlines():
        if re.match(r"^parts:\s*$", line):
            in_parts = True
            continue
        if in_parts:
            m = re.match(r"^\s+([A-Za-z0-9_]+)\s*:\s*(.+?)\s*$", line)
            if m:
                parts[m.group(1)] = m.group(2)
            elif line.strip() and not line.strip().startswith("#"):
                in_parts = False
    return parts


# ---------------------------------------------------------------- code samples

def lang_label(lang):
    return LANG_LABELS.get(lang.lower(), lang.upper() if lang else "Code")


def highlight(code, lang):
    if HAS_PYGMENTS:
        try:
            lexer = get_lexer_by_name(PYG_LANGS.get(lang.lower(), lang.lower() or "text"))
        except ClassNotFound:
            lexer = TextLexer()
        return pyg_highlight(code, lexer, HtmlFormatter(nowrap=False))
    return f'<pre><code class="language-{lang}">{html_mod.escape(code)}</code></pre>'


def extract_samples(body):
    """Pull :::sample blocks out of the markdown, leave placeholder tokens."""
    panes_per_sample = []

    def repl(match):
        inner = match.group(1)
        panes = [
            (m.group(1) or "text", m.group(2))
            for m in FENCE_RE.finditer(inner)
        ]
        panes_per_sample.append(panes)
        return f"\n\n{TOKEN_FMT.format(len(panes_per_sample) - 1)}\n\n"

    return SAMPLE_RE.sub(repl, body), panes_per_sample


def sample_html(panes):
    if not panes:
        return ""
    langs = [p[0].lower() for p in panes]
    tabs, bodies = [], []
    for i, (lang, code) in enumerate(panes):
        active = " active" if i == 0 else ""
        tabs.append(
            f'<button type="button" class="code-tab{active}" data-lang="{html_mod.escape(lang.lower())}">'
            f"{html_mod.escape(lang_label(lang))}</button>"
        )
        hidden = "" if i == 0 else " hidden"
        bodies.append(
            f'<div class="code-pane{active}" data-lang="{html_mod.escape(lang.lower())}"{hidden}>'
            f"{highlight(code, lang)}</div>"
        )
    return (
        f'<div class="code-sample" data-langs="{" ".join(langs)}">'
        f'<div class="code-tabs">{"".join(tabs)}'
        '<button type="button" class="code-copy" title="Copy code">Copy</button></div>'
        f"{''.join(bodies)}</div>"
    )


def inject_samples(rendered, panes_per_sample):
    for i, panes in enumerate(panes_per_sample):
        token = TOKEN_FMT.format(i)
        block = sample_html(panes)
        rendered = rendered.replace(f"<p>{token}</p>", block).replace(token, block)
    return rendered


# ---------------------------------------------------------------- pages

def discover_pages():
    pages = []
    for f in sorted(PAGES_DIR.glob("*.md")):
        m = re.match(r"^(\d+)-(.+)\.md$", f.name)
        order = int(m.group(1)) if m else 0
        slug = m.group(2) if m else f.stem
        meta, body = parse_front(f.read_text(encoding="utf-8"))
        meta.setdefault("title", slug.replace("-", " ").title())
        pages.append({
            "file": f, "order": order, "slug": slug, "out": f"{slug}.html",
            "meta": meta, "body": body, "is_index": f.stem == "index",
        })
    pages.sort(key=lambda p: (0 if p["is_index"] else 1, p["order"], p["slug"]))
    return pages


def md_render(body):
    md = markdown.Markdown(
        extensions=["fenced_code", "tables", "attr_list", "toc", "admonition",
                    "sane_lists", "codehilite"],
        extension_configs={
            "codehilite": {"css_class": "highlight", "guess_lang": False},
            "toc": {"separator": "-"},
        },
    )
    stripped, panes = extract_samples(body)
    return inject_samples(md.convert(stripped), panes)


def badge_html(meta):
    out = []
    if meta.get("level"):
        lvl = meta["level"].lower()
        out.append(f'<span class="badge badge-level badge-{html_mod.escape(lvl)}">'
                   f"{LEVEL_LABELS.get(lvl, lvl.title())}</span>")
    if meta.get("oracle"):
        out.append(f'<span class="badge badge-oracle">Oracle ≥ {html_mod.escape(meta["oracle"])}</span>')
    for d in meta.get("demos", []):
        out.append(f'<span class="badge badge-demo">demo/{html_mod.escape(d)}</span>')
    if meta.get("sql"):
        s = html_mod.escape(meta["sql"])
        out.append(f'<a class="badge badge-demo" href="assets/sql/{s}">{s}</a>')
    return f'<div class="badges">{"".join(out)}</div>' if out else ""


def page_label(page):
    """Sidebar/prev-next label: navtitle if set, else the full title."""
    return page["meta"].get("navtitle") or page["meta"]["title"]


def nav_html(parts, pages, current):
    by_part = {}
    for p in pages:
        if not p["is_index"]:
            by_part.setdefault(p["meta"].get("part", ""), []).append(p)

    lis = []
    items = []
    for p in pages:
        if p["is_index"]:
            active = ' class="active"' if p["out"] == current else ""
            items.append(f'<li{active}><a href="{p["out"]}">{html_mod.escape(page_label(p))}</a></li>')
    lis.append(f'<ul class="nav-group nav-top">{"".join(items)}</ul>')

    for key, name in parts.items():
        group = by_part.get(key)
        if not group:
            continue
        sub, has_active = [], False
        for p in group:
            active = ' class="active"' if p["out"] == current else ""
            has_active = has_active or bool(active)
            sub.append(f'<li{active}><a href="{p["out"]}">{html_mod.escape(page_label(p))}</a></li>')
        open_attr = ' open' if has_active else ''
        lis.append(f'<details class="nav-details" data-part="{html_mod.escape(str(key))}"{open_attr}>'
                   f'<summary class="nav-part">{html_mod.escape(name)}</summary>'
                   f'<ul class="nav-group">{"".join(sub)}</ul></details>')
    return "".join(lis)


def prev_next(pages, page):
    seq = [p for p in pages]
    i = seq.index(page)
    prevp = seq[i - 1] if i > 0 else None
    nextp = seq[i + 1] if i < len(seq) - 1 else None
    prev_h = (f'<a class="pn-link pn-prev" href="{prevp["out"]}">'
              f'<span class="pn-dir">← Previous</span>{html_mod.escape(page_label(prevp))}</a>') if prevp else "<span></span>"
    next_h = (f'<a class="pn-link pn-next" href="{nextp["out"]}">'
              f'<span class="pn-dir">Next →</span>{html_mod.escape(page_label(nextp))}</a>') if nextp else "<span></span>"
    return f'<nav class="prev-next">{prev_h}{next_h}</nav>'


TEMPLATE = """<!DOCTYPE html>
<html lang="en" data-theme="light">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>{title} — {site}</title>
<link rel="stylesheet" href="assets/tutorials.css">
<link rel="stylesheet" href="assets/tutorials-dark.css">
<link rel="stylesheet" href="assets/pygments.css">
<link rel="stylesheet" href="assets/pygments-dark.css">
<script src="assets/tutorials.js" defer></script>
</head>
<body>
<header class="topbar">
  <button id="nav-toggle" class="icon-btn" aria-label="Toggle navigation">☰</button>
  <a class="brand" href="index.html">{site} <span class="brand-ver">{ver}</span></a>
  <div class="topbar-actions">
    <button id="theme-toggle" class="icon-btn" aria-label="Toggle dark theme">◐</button>
  </div>
</header>
<div class="layout">
  <nav class="sidebar" id="sidebar">{nav}</nav>
  <div class="sidebar-scrim" id="sidebar-scrim"></div>
  <main class="content">
    {badges}
    {content}
    {prevnext}
    <footer class="page-footer">OCILIB {ver} — C/C++ driver for Oracle —
    generated from <code>doc/tutorials/pages</code></footer>
  </main>
</div>
</body>
</html>
"""


# ---------------------------------------------------------------- build

def write_pygments_css():
    if not HAS_PYGMENTS:
        (OUT_DIR / "assets" / "pygments.css").write_text("/* pygments not installed */\n")
        (OUT_DIR / "assets" / "pygments-dark.css").write_text("/* pygments not installed */\n")
        return
    light = HtmlFormatter(style="default").get_style_defs(".highlight")
    dark = HtmlFormatter(style="monokai").get_style_defs('html[data-theme="dark"] .highlight')
    (OUT_DIR / "assets" / "pygments.css").write_text(light, encoding="utf-8")
    (OUT_DIR / "assets" / "pygments-dark.css").write_text(dark, encoding="utf-8")


def build():
    if OUT_DIR.exists():
        shutil.rmtree(OUT_DIR)
    shutil.copytree(ASSETS_DIR, OUT_DIR / "assets")

    parts = parse_parts()
    pages = discover_pages()
    write_pygments_css()

    for page in pages:
        content = md_render(page["body"])
        page_html = (TEMPLATE
                     .replace("{title}", html_mod.escape(page["meta"]["title"]))
                     .replace("{site}", SITE_TITLE)
                     .replace("{ver}", SITE_VERSION)
                     .replace("{nav}", nav_html(parts, pages, page["out"]))
                     .replace("{badges}", badge_html(page["meta"]))
                     .replace("{content}", content)
                     .replace("{prevnext}", prev_next(pages, page)))
        (OUT_DIR / page["out"]).write_text(page_html, encoding="utf-8")
        print(f"  {page['file'].name} -> html/{page['out']}")

    print(f"Built {len(pages)} page(s) into {OUT_DIR}")
    return pages


def check_links(pages):
    ok = True
    valid = {p["out"] for p in pages}
    for p in pages:
        rendered = (OUT_DIR / p["out"]).read_text(encoding="utf-8")
        for href in re.findall(r'href="([^"#]+?)(?:#[^"]*)?"', rendered):
            if href.startswith(("http", "mailto:", "assets/")):
                continue
            if href not in valid:
                print(f"  BROKEN LINK in {p['out']}: {href}")
                ok = False
    print("Link check:", "OK" if ok else "FAILURES found")
    return ok


if __name__ == "__main__":
    built = build()
    if "--check" in sys.argv:
        sys.exit(0 if check_links(built) else 1)
