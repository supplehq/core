/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * Based on LLVM/Clang.
 *
 * This file is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 */

#ifndef PLUGINHANDLER_H
#define PLUGINHANDLER_H

#include <cstddef>
#include <functional>
#include <memory>
#include <set>
#include <unordered_map>

#include <clang/AST/ASTConsumer.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Rewrite/Core/Rewriter.h>

using namespace clang;

namespace std {

template<> struct hash<::clang::SourceLocation> {
    size_t operator ()(::clang::SourceLocation loc) const
    { return loc.getRawEncoding(); }
};

}

namespace loplugin
{

class Plugin;
struct InstantiationData;

/**
 Class that manages all LO modules.
*/
class PluginHandler
    : public ASTConsumer
{
public:
    PluginHandler( CompilerInstance& compiler, const std::vector< std::string >& args );
    virtual ~PluginHandler();
    virtual void HandleTranslationUnit( ASTContext& context ) override;
    static void registerPlugin( Plugin* (*create)( const InstantiationData& ), const char* optionName, bool isPPCallback, bool byDefault );
    DiagnosticBuilder report( DiagnosticsEngine::Level level, const char * plugin, StringRef message,
            CompilerInstance& compiler, SourceLocation loc = SourceLocation());
    bool ignoreLocation(SourceLocation loc);
    bool addRemoval( SourceLocation loc );
    bool isDebugMode() const { return debugMode; }
    static bool isUnitTestMode();
private:
    void handleOption( const std::string& option );
    void createPlugins( std::set< std::string > rewriters );
    DiagnosticBuilder report( DiagnosticsEngine::Level level, StringRef message, SourceLocation loc = SourceLocation());
    bool checkIgnoreLocation(SourceLocation loc);
    CompilerInstance& compiler;
    StringRef const mainFileName;
    std::unordered_map<SourceLocation, bool> ignored_;
    Rewriter rewriter;
    std::set< SourceLocation > removals;
    std::string scope;
    std::string warningsOnly;
    bool warningsAsErrors;
    bool debugMode = false;
};

/**
 The Clang plugin class, just forwards to PluginHandler.
*/
class LibreOfficeAction
    : public PluginASTAction
{
public:
#if CLANG_VERSION >= 30600
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer( CompilerInstance& Compiler, StringRef InFile );
#else
    virtual ASTConsumer* CreateASTConsumer( CompilerInstance& Compiler, StringRef InFile );
#endif

    virtual bool ParseArgs( const CompilerInstance& CI, const std::vector< std::string >& args );
private:
    std::vector< std::string > _args;
};

} // namespace

#endif // COMPILEPLUGIN_H

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
