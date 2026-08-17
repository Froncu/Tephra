#ifndef CONSUMER_HPP
#define CONSUMER_HPP

#include "pch.hpp"
#include "visitor.hpp"

namespace tep
{
   class Consumer final : public clang::ASTConsumer
   {
      public:
         explicit Consumer(clang::SourceManager const& source_manager);
         Consumer(Consumer const&) = default;
         Consumer(Consumer&&) = default;

         ~Consumer() override = default;

         auto operator=(Consumer const&) -> Consumer& = delete;
         auto operator=(Consumer&&) -> Consumer& = delete;

         auto HandleTopLevelDecl(clang::DeclGroupRef declarations) -> bool override;

      private:
         Visitor visitor_;
   };
}

#endif