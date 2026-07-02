#include "HTMLGenerator.h"
#include <sstream>

namespace Markdown {

std::string HTMLGenerator::generateSnippet(const std::vector<MarkdownBlock>& blocks) {
    std::stringstream html;
    
    bool inUnorderedList = false;
    bool inOrderedList = false;

    auto closeLists = [&]() {
        if (inUnorderedList) {
            html << "</ul>\n";
            inUnorderedList = false;
        }
        if (inOrderedList) {
            html << "</ol>\n";
            inOrderedList = false;
        }
    };

    for (const auto& block : blocks) {
        // Manage lists transitions
        if (block.type == BlockType::UnorderedListItem) {
            if (inOrderedList) {
                html << "</ol>\n";
                inOrderedList = false;
            }
            if (!inUnorderedList) {
                html << "<ul>\n";
                inUnorderedList = true;
            }
            html << "  <li>" << MarkdownParser::parseInline(block.content) << "</li>\n";
            continue;
        } else if (block.type == BlockType::OrderedListItem) {
            if (inUnorderedList) {
                html << "</ul>\n";
                inUnorderedList = false;
            }
            if (!inOrderedList) {
                html << "<ol>\n";
                inOrderedList = true;
            }
            html << "  <li>" << MarkdownParser::parseInline(block.content) << "</li>\n";
            continue;
        } else {
            // Close lists if we hit any other block
            closeLists();
        }

        // Render individual block types
        switch (block.type) {
            case BlockType::Heading1:
                html << "<h1>" << MarkdownParser::parseInline(block.content) << "</h1>\n";
                break;
            case BlockType::Heading2:
                html << "<h2>" << MarkdownParser::parseInline(block.content) << "</h2>\n";
                break;
            case BlockType::Heading3:
                html << "<h3>" << MarkdownParser::parseInline(block.content) << "</h3>\n";
                break;
            case BlockType::Heading4:
                html << "<h4>" << MarkdownParser::parseInline(block.content) << "</h4>\n";
                break;
            case BlockType::Heading5:
                html << "<h5>" << MarkdownParser::parseInline(block.content) << "</h5>\n";
                break;
            case BlockType::Heading6:
                html << "<h6>" << MarkdownParser::parseInline(block.content) << "</h6>\n";
                break;
            case BlockType::HorizontalRule:
                html << "<hr />\n";
                break;
            case BlockType::Blockquote: {
                std::string content = MarkdownParser::parseInline(block.content);
                // Replace internal newlines with break tags inside blockquote
                size_t pos = 0;
                while ((pos = content.find("\n", pos)) != std::string::npos) {
                    content.replace(pos, 1, "<br />");
                    pos += 6;
                }
                html << "<blockquote>\n  " << content << "\n</blockquote>\n";
                break;
            }
            case BlockType::CodeBlock: {
                std::string codeHtml = MarkdownParser::escapeHtml(block.content);
                html << "<pre><code";
                if (!block.info.empty()) {
                    html << " class=\"language-" << block.info << "\"";
                }
                html << ">" << codeHtml << "</code></pre>\n";
                break;
            }
            case BlockType::Paragraph:
                html << "<p>" << MarkdownParser::parseInline(block.content) << "</p>\n";
                break;
            default:
                break;
        }
    }

    // Close any lists remaining at the end
    closeLists();

    return html.str();
}

std::string HTMLGenerator::generateDocument(const std::vector<MarkdownBlock>& blocks, Theme theme, const std::string& title) {
    std::stringstream doc;
    doc << "<!DOCTYPE html>\n";
    doc << "<html>\n";
    doc << "<head>\n";
    doc << "  <meta charset=\"UTF-8\">\n";
    doc << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    doc << "  <title>" << title << "</title>\n";
    doc << "  <style>\n" << getCSS(theme) << "\n  </style>\n";
    doc << "</head>\n";
    doc << "<body>\n";
    doc << "  <div class=\"container\">\n";
    doc << generateSnippet(blocks);
    doc << "  </div>\n";
    doc << "</body>\n";
    doc << "</html>\n";
    return doc.str();
}

std::string HTMLGenerator::getCSS(Theme theme) {
    if (theme == Theme::Light) {
        return R"(
            :root {
                --bg-color: #ffffff;
                --text-color: #24292f;
                --link-color: #0969da;
                --heading-color: #1f2328;
                --border-color: #d0d7de;
                --code-bg: #afb8c133;
                --pre-bg: #f6f8fa;
                --blockquote-color: #57606a;
                --blockquote-border: #d0d7de;
                --font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif;
            }
            body {
                background-color: var(--bg-color);
                color: var(--text-color);
                font-family: var(--font-family);
                font-size: 16px;
                line-height: 1.6;
                word-wrap: break-word;
                margin: 0;
                padding: 2rem 1rem;
            }
            .container {
                max-width: 800px;
                margin: 0 auto;
                padding: 0 1rem;
            }
            h1, h2, h3, h4, h5, h6 {
                color: var(--heading-color);
                font-weight: 600;
                margin-top: 24px;
                margin-bottom: 16px;
                line-height: 1.25;
            }
            h1 { font-size: 2em; border-bottom: 1px solid var(--border-color); padding-bottom: 0.3em; }
            h2 { font-size: 1.5em; border-bottom: 1px solid var(--border-color); padding-bottom: 0.3em; }
            h3 { font-size: 1.25em; }
            h4 { font-size: 1em; }
            h5 { font-size: 0.875em; }
            h6 { font-size: 0.85em; }
            p { margin-top: 0; margin-bottom: 16px; }
            a { color: var(--link-color); text-decoration: none; }
            a:hover { text-decoration: underline; }
            hr {
                height: 0.25em;
                padding: 0;
                margin: 24px 0;
                background-color: var(--border-color);
                border: 0;
            }
            blockquote {
                padding: 0 1em;
                color: var(--blockquote-color);
                border-left: 0.25em solid var(--blockquote-border);
                margin: 0 0 16px 0;
            }
            code {
                padding: 0.2em 0.4em;
                margin: 0;
                font-size: 85%;
                background-color: var(--code-bg);
                border-radius: 6px;
                font-family: ui-monospace, SFMono-Regular, SF Mono, Menlo, Consolas, Liberation Mono, monospace;
            }
            pre {
                padding: 16px;
                overflow: auto;
                font-size: 85%;
                line-height: 1.45;
                background-color: var(--pre-bg);
                border-radius: 6px;
                margin-top: 0;
                margin-bottom: 16px;
            }
            pre code {
                background-color: transparent;
                padding: 0;
                margin: 0;
                font-size: 100%;
                border-radius: 0;
                word-break: normal;
                white-space: pre;
            }
            ul, ol {
                padding-left: 2em;
                margin-top: 0;
                margin-bottom: 16px;
            }
            li { margin-top: 0.25em; }
            img {
                max-width: 100%;
                box-sizing: content-box;
                background-color: var(--bg-color);
            }
        )";
    } else {
        // Dark Mode Theme
        return R"(
            :root {
                --bg-color: #0d1117;
                --text-color: #c9d1d9;
                --link-color: #58a6ff;
                --heading-color: #f0f6fc;
                --border-color: #30363d;
                --code-bg: #6e768166;
                --pre-bg: #161b22;
                --blockquote-color: #8b949e;
                --blockquote-border: #30363d;
                --font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Helvetica, Arial, sans-serif;
            }
            body {
                background-color: var(--bg-color);
                color: var(--text-color);
                font-family: var(--font-family);
                font-size: 16px;
                line-height: 1.6;
                word-wrap: break-word;
                margin: 0;
                padding: 2rem 1rem;
            }
            .container {
                max-width: 800px;
                margin: 0 auto;
                padding: 0 1rem;
            }
            h1, h2, h3, h4, h5, h6 {
                color: var(--heading-color);
                font-weight: 600;
                margin-top: 24px;
                margin-bottom: 16px;
                line-height: 1.25;
            }
            h1 { font-size: 2em; border-bottom: 1px solid var(--border-color); padding-bottom: 0.3em; }
            h2 { font-size: 1.5em; border-bottom: 1px solid var(--border-color); padding-bottom: 0.3em; }
            h3 { font-size: 1.25em; }
            h4 { font-size: 1em; }
            h5 { font-size: 0.875em; }
            h6 { font-size: 0.85em; }
            p { margin-top: 0; margin-bottom: 16px; }
            a { color: var(--link-color); text-decoration: none; }
            a:hover { text-decoration: underline; }
            hr {
                height: 0.25em;
                padding: 0;
                margin: 24px 0;
                background-color: var(--border-color);
                border: 0;
            }
            blockquote {
                padding: 0 1em;
                color: var(--blockquote-color);
                border-left: 0.25em solid var(--blockquote-border);
                margin: 0 0 16px 0;
            }
            code {
                padding: 0.2em 0.4em;
                margin: 0;
                font-size: 85%;
                background-color: var(--code-bg);
                border-radius: 6px;
                font-family: ui-monospace, SFMono-Regular, SF Mono, Menlo, Consolas, Liberation Mono, monospace;
            }
            pre {
                padding: 16px;
                overflow: auto;
                font-size: 85%;
                line-height: 1.45;
                background-color: var(--pre-bg);
                border-radius: 6px;
                margin-top: 0;
                margin-bottom: 16px;
            }
            pre code {
                background-color: transparent;
                padding: 0;
                margin: 0;
                font-size: 100%;
                border-radius: 0;
                word-break: normal;
                white-space: pre;
            }
            ul, ol {
                padding-left: 2em;
                margin-top: 0;
                margin-bottom: 16px;
            }
            li { margin-top: 0.25em; }
            img {
                max-width: 100%;
                box-sizing: content-box;
                background-color: var(--bg-color);
            }
        )";
    }
}

} // namespace Markdown
