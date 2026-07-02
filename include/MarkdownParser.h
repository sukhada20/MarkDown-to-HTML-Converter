#ifndef MARKDOWNPARSER_H
#define MARKDOWNPARSER_H

#include <string>
#include <vector>

namespace Markdown {

enum class BlockType {
    Paragraph,
    Heading1,
    Heading2,
    Heading3,
    Heading4,
    Heading5,
    Heading6,
    Blockquote,
    CodeBlock,
    UnorderedListItem,
    OrderedListItem,
    HorizontalRule
};

struct MarkdownBlock {
    BlockType type;
    std::string content; // The text content of the block
    std::string info;    // Additional info: e.g. language for CodeBlock
};

class MarkdownParser {
public:
    MarkdownParser() = default;
    ~MarkdownParser() = default;

    // Parses raw markdown text into a sequence of blocks
    std::vector<MarkdownBlock> parse(const std::string& markdownText);

    // Processes inline elements (bold, italic, links, images, inline code)
    // and returns the HTML-formatted content.
    static std::string parseInline(const std::string& text);

    // Escapes special HTML characters (&, <, >, ", ')
    static std::string escapeHtml(const std::string& text);
};

} // namespace Markdown

#endif // MARKDOWNPARSER_H
