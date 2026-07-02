#ifndef HTMLGENERATOR_H
#define HTMLGENERATOR_H

#include <string>
#include <vector>
#include "MarkdownParser.h"

namespace Markdown {

class HTMLGenerator {
public:
    enum class Theme {
        Light,
        Dark
    };

    HTMLGenerator() = default;
    ~HTMLGenerator() = default;

    // Generates a complete HTML document including head, body, and modern CSS
    std::string generateDocument(const std::vector<MarkdownBlock>& blocks, 
                                 Theme theme = Theme::Light, 
                                 const std::string& title = "Converted Document");

    // Generates just the inner HTML fragment representing the blocks
    std::string generateSnippet(const std::vector<MarkdownBlock>& blocks);

private:
    // Returns the CSS stylesheet according to the theme
    static std::string getCSS(Theme theme);
};

} // namespace Markdown

#endif // HTMLGENERATOR_H
