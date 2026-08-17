#include <clang/Basic/Version.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/raw_ostream.h>

#include <array>
#include <optional>
#include <regex>
#include <set>
#include <string>

namespace
{
   auto read_file(llvm::StringRef const path) -> std::string
   {
      auto const buffer = llvm::MemoryBuffer::getFile(path);
      return buffer ? buffer.get()->getBuffer().str() : std::string{};
   }

   auto compiler_version(std::string const& compiler) -> std::optional<std::string>
   {
      auto const compiler_path = llvm::sys::findProgramByName(compiler);
      if (!compiler_path)
      {
         llvm::errs() << "Tephra could not find compiler '" << compiler << "'.\n";
         return std::nullopt;
      }

      llvm::SmallString<128> output_path;
      llvm::SmallString<128> error_path;
      if (llvm::sys::fs::createTemporaryFile("tephra-compiler-version", "out", output_path)
         || llvm::sys::fs::createTemporaryFile("tephra-compiler-version", "err", error_path))
      {
         llvm::errs() << "Tephra could not create temporary files for the compiler version check.\n";
         return std::nullopt;
      }

      std::array<llvm::StringRef, 2> const arguments{ *compiler_path, "--version" };
      std::array<std::optional<llvm::StringRef>, 3> const redirects{
         std::nullopt,
         output_path.str(),
         error_path.str()
      };

      std::string execution_error;
      auto const result = llvm::sys::ExecuteAndWait(
         *compiler_path,
         arguments,
         std::nullopt,
         redirects,
         0,
         0,
         &execution_error);

      auto const output = read_file(output_path);
      auto const error = read_file(error_path);
      llvm::sys::fs::remove(output_path);
      llvm::sys::fs::remove(error_path);

      if (result != 0)
      {
         llvm::errs() << "Tephra could not query compiler '" << *compiler_path << "': "
            << execution_error << error << '\n';
         return std::nullopt;
      }

      std::smatch match;
      std::regex const pattern{ "clang version ([0-9]+\\.[0-9]+\\.[0-9]+([-+][0-9A-Za-z.-]+)?)" };
      if (!std::regex_search(output, match, pattern))
      {
         llvm::errs() << "Tephra requires Clang " << CLANG_VERSION_STRING
            << ", but could not identify '" << *compiler_path << "' as Clang.\n";
         return std::nullopt;
      }

      return match[1].str();
   }

   auto compatible_compilers() -> bool
   {
      std::string database_error;
      auto const database = clang::tooling::CompilationDatabase::autoDetectFromDirectory(".", database_error);
      if (!database)
      {
         llvm::errs() << "Tephra could not find compile_commands.json in the current directory "
            "or any parent directory: "
            << database_error << '\n';
         return false;
      }

      auto const commands = database->getAllCompileCommands();
      if (commands.empty())
      {
         llvm::errs() << "Tephra found an empty compilation database.\n";
         return false;
      }

      std::set<std::string> compilers;
      for (auto const& command : commands)
      {
         if (command.CommandLine.empty())
         {
            llvm::errs() << "Tephra found a compile command without a compiler.\n";
            return false;
         }
         compilers.insert(command.CommandLine.front());
      }

      for (auto const& compiler : compilers)
      {
         auto const version = compiler_version(compiler);
         if (!version)
            return false;

         if (*version != CLANG_VERSION_STRING)
         {
            llvm::errs() << "Tephra requires Clang " << CLANG_VERSION_STRING << ", but compiler '"
               << compiler << "' reports Clang " << *version << ".\n";
            return false;
         }
      }

      return true;
   }
}

auto main() -> int
{
   if (!compatible_compilers())
      return 1;

   llvm::outs() << "wassup!";
   return 0;
}