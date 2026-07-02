#include "MarkdownParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace Markdown {

// Helper to trim trailing whitespace
static std::string trimRight(const std::string& s) {
    size_t end = s.find_last_not_of(" \t\r\n");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

// Helper to trim leading whitespace
static std::string trimLeft(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start);
}

// Helper to trim both ends
static std::string trim(const std::string& s) {
    return trimLeft(trimRight(s));
}

std::string MarkdownParser::escapeHtml(const std::string& text) {
    std::string escaped;
    escaped.reserve(text.size() * 1.1);
    for (char c : text) {
        switch (c) {
            case '&':  escaped.append("&amp;");  break;
            case '<':  escaped.append("&lt;");   break;
            case '>':  escaped.append("&gt;");   break;
            case '"':  escaped.append("&quot;"); break;
            case '\'': escaped.append("&#39;");  break;
            default:   escaped.push_back(c);     break;
        }
    }
    return escaped;
}

std::vector<MarkdownBlock> MarkdownParser::parse(const std::string& markdownText) {
    std::vector<MarkdownBlock> blocks;
    std::vector<std::string> lines;
    
    // Split input into lines
    std::stringstream ss(markdownText);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }

    bool inCodeBlock = false;
    std::string codeBlockLanguage = "";
    std::string codeBlockContent = "";

    for (size_t i = 0; i < lines.size(); ++i) {
        const auto& currentLine = lines[i];
        std::string trimmedLine = trim(currentLine);

        if (inCodeBlock) {
            // Check for ending triple-backticks
            if (trimmedLine.rfind("```", 0) == 0 && trimmedLine.size() == 3) {
                if (!codeBlockContent.empty() && codeBlockContent.back() == '\n') {
                    codeBlockContent.pop_back();
                }
                
                MarkdownBlock block;
                block.type = BlockType::CodeBlock;
                block.content = codeBlockContent;
                block.info = codeBlockLanguage;
                blocks.push_back(block);

                inCodeBlock = false;
                codeBlockContent = "";
                codeBlockLanguage = "";
            } else {
                codeBlockContent.append(currentLine).append("\n");
            }
            continue;
        }

        // Code block start
        if (trimmedLine.rfind("```", 0) == 0) {
            inCodeBlock = true;
            codeBlockLanguage = trim(trimmedLine.substr(3));
            codeBlockContent = "";
            continue;
        }

        // Empty lines separator
        if (trimmedLine.empty()) {
            continue;
        }

        // Horizontal Rule (---, ***, ___)
        if ((trimmedLine == "---" || trimmedLine == "***" || trimmedLine == "___") ||
            (trimmedLine.size() >= 3 && std::all_of(trimmedLine.begin(), trimmedLine.end(), [](char c){ return c == '-'; })) ||
            (trimmedLine.size() >= 3 && std::all_of(trimmedLine.begin(), trimmedLine.end(), [](char c){ return c == '*'; })) ||
            (trimmedLine.size() >= 3 && std::all_of(trimmedLine.begin(), trimmedLine.end(), [](char c){ return c == '_'; }))) {
            MarkdownBlock block;
            block.type = BlockType::HorizontalRule;
            blocks.push_back(block);
            continue;
        }

        // Headings (# Heading)
        if (trimmedLine[0] == '#') {
            size_t headingLevel = 0;
            while (headingLevel < trimmedLine.size() && trimmedLine[headingLevel] == '#') {
                headingLevel++;
            }
            if (headingLevel >= 1 && headingLevel <= 6 && headingLevel < trimmedLine.size() && trimmedLine[headingLevel] == ' ') {
                MarkdownBlock block;
                block.type = static_cast<BlockType>(static_cast<int>(BlockType::Heading1) + headingLevel - 1);
                block.content = trim(trimmedLine.substr(headingLevel + 1));
                blocks.push_back(block);
                continue;
            }
        }

        // Blockquotes (> Quote)
        if (trimmedLine[0] == '>') {
            std::string content = trimmedLine.substr(1);
            if (!content.empty() && content[0] == ' ') {
                content = content.substr(1);
            }
            
            if (!blocks.empty() && blocks.back().type == BlockType::Blockquote) {
                blocks.back().content.append("\n").append(content);
            } else {
                MarkdownBlock block;
                block.type = BlockType::Blockquote;
                block.content = content;
                blocks.push_back(block);
            }
            continue;
        }

        // Unordered List Items (- Item, * Item, + Item)
        if ((trimmedLine.rfind("- ", 0) == 0 || trimmedLine.rfind("* ", 0) == 0 || trimmedLine.rfind("+ ", 0) == 0)) {
            MarkdownBlock block;
            block.type = BlockType::UnorderedListItem;
            block.content = trim(trimmedLine.substr(2));
            blocks.push_back(block);
            continue;
        }

        // Ordered List Items (1. Item)
        size_t dotPos = trimmedLine.find('.');
        if (dotPos != std::string::npos && dotPos > 0 && dotPos < 10 && dotPos < trimmedLine.size() - 1 && trimmedLine[dotPos + 1] == ' ') {
            bool onlyDigits = true;
            for (size_t idx = 0; idx < dotPos; ++idx) {
                if (!std::isdigit(trimmedLine[idx])) {
                    onlyDigits = false;
                    break;
                }
            }
            if (onlyDigits) {
                MarkdownBlock block;
                block.type = BlockType::OrderedListItem;
                block.content = trim(trimmedLine.substr(dotPos + 2));
                block.info = trimmedLine.substr(0, dotPos);
                blocks.push_back(block);
                continue;
            }
        }

        // Paragraph - merge consecutive text lines
        if (!blocks.empty() && blocks.back().type == BlockType::Paragraph) {
            blocks.back().content.append(" ").append(trimmedLine);
        } else {
            MarkdownBlock block;
            block.type = BlockType::Paragraph;
            block.content = trimmedLine;
            blocks.push_back(block);
        }
    }

    // Capture unclosed code block
    if (inCodeBlock) {
        MarkdownBlock block;
        block.type = BlockType::CodeBlock;
        block.content = codeBlockContent;
        block.info = codeBlockLanguage;
        blocks.push_back(block);
    }

    return blocks;
}

std::string MarkdownParser::parseInline(const std::string& text) {
    std::string result = escapeHtml(text);

    std::vector<std::string> placeholders;
    auto addPlaceholder = [&](const std::string& val) -> std::string {
        placeholders.push_back(val);
        return "\x01PH" + std::to_string(placeholders.size() - 1) + "\x02";
    };

    std::smatch match;

    // 1. Extract Inline Code: `code`
    std::regex inlineCodeRegex("`([^`]+)`");
    while (std::regex_search(result, match, inlineCodeRegex)) {
        std::string codeHtml = "<code>" + match[1].str() + "</code>";
        std::string ph = addPlaceholder(codeHtml);
        result.replace(match.position(0), match.length(0), ph);
    }

    // 2. Extract Images: ![Alt](url)
    std::regex imageRegex("!\\[([^\\]]*)\\]\\(([^\\)]*)\\)");
    while (std::regex_search(result, match, imageRegex)) {
        std::string imgHtml = "<img src=\"" + match[2].str() + "\" alt=\"" + match[1].str() + "\" />";
        std::string ph = addPlaceholder(imgHtml);
        result.replace(match.position(0), match.length(0), ph);
    }

    // 3. Extract Links: [Text](url) (text can contain styles recursively)
    std::regex linkRegex("\\[([^\\]]*)\\]\\(([^\\)]*)\\)");
    while (std::regex_search(result, match, linkRegex)) {
        std::string linkText = parseInline(match[1].str());
        std::string linkHtml = "<a href=\"" + match[2].str() + "\">" + linkText + "</a>";
        std::string ph = addPlaceholder(linkHtml);
        result.replace(match.position(0), match.length(0), ph);
    }

    // 4. Bold + Italic: ***text*** or ___text___
    std::regex boldItalicStarRegex("\\*\\*\\*([^*]+)\\*\\*\\*");
    result = std::regex_replace(result, boldItalicStarRegex, "<strong><em>$1</em></strong>");
    std::regex boldItalicUnderRegex("___([^_]+)___");
    result = std::regex_replace(result, boldItalicUnderRegex, "<strong><em>$1</em></strong>");

    // 5. Bold: **text** or __text__
    std::regex boldStarRegex("\\*\\*([^*]+)\\*\\*");
    result = std::regex_replace(result, boldStarRegex, "<strong>$1</strong>");
    std::regex boldUnderRegex("__([^_]+)__");
    result = std::regex_replace(result, boldUnderRegex, "<strong>$1</strong>");

    // 6. Italic: *text* or _text_
    std::regex italicStarRegex("\\*([^*]+)\\*");
    result = std::regex_replace(result, italicStarRegex, "<em>$1</em>");
    std::regex italicUnderRegex("_([^_]+)_");
    result = std::regex_replace(result, italicUnderRegex, "<em>$1</em>");

    // 7. Restore placeholders
    for (int i = static_cast<int>(placeholders.size()) - 1; i >= 0; --i) {
        std::string phKey = "\x01PH" + std::to_string(i) + "\x02";
        size_t pos = result.find(phKey);
        while (pos != std::string::npos) {
            result.replace(pos, phKey.length(), placeholders[i]);
            pos = result.find(phKey);
        }
    }

    return result;
}

} // namespace Markdown
