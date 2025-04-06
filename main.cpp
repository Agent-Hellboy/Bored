#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"
#include "clang/Basic/TokenKinds.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TokenKinds.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

// ASTConsumer that tokenizes the main file and prints tokens.
class KeywordASTConsumer : public clang::ASTConsumer {
public:
    explicit KeywordASTConsumer(clang::CompilerInstance &CI)
        : CI(CI) {}

    void HandleTranslationUnit(clang::ASTContext &Context) override {
        clang::SourceManager &SM = Context.getSourceManager();
        clang::FileID FID = SM.getMainFileID();

        llvm::MemoryBufferRef BufferRef = SM.getBufferOrFake(FID);

        clang::LangOptions LangOpts;
        LangOpts.CPlusPlus = true;

        clang::Lexer Lex(FID, BufferRef, SM, LangOpts);
        clang::Token Tok;
        while (!Lex.LexFromRawLexer(Tok)) {
            if (Tok.is(clang::tok::eof)) break;
        
            clang::SourceLocation Loc = Tok.getLocation();
            unsigned line = SM.getSpellingLineNumber(Loc);
            unsigned col  = SM.getSpellingColumnNumber(Loc);
        
            llvm::StringRef Spelling = clang::Lexer::getSpelling(Tok, SM, CI.getLangOpts());
        
            llvm::outs() << "Token: " << clang::tok::getTokenName(Tok.getKind())
                      << " -> '" << Spelling.str() << "'"
                      << " at Line " << line << ", Column " << col << "\n";
        }
    }

private:
    clang::CompilerInstance &CI;
};

// FrontendAction that creates our ASTConsumer.
class KeywordFrontendAction : public clang::ASTFrontendAction {
public:
    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI,
                                                           llvm::StringRef InFile) override {
        return std::make_unique<KeywordASTConsumer>(CI);
    }
};

static llvm::cl::OptionCategory MyToolCategory("keyword-extractor options");

int main(int argc, const char **argv) {
    auto ExpectedParser =
        clang::tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);

    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }

    clang::tooling::CommonOptionsParser &OptionsParser = ExpectedParser.get();
    clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                   OptionsParser.getSourcePathList());

    return Tool.run(clang::tooling::newFrontendActionFactory<KeywordFrontendAction>().get());
}
