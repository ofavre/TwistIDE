#ifndef _TARGETOPTIONSBUILDER_H
#define _TARGETOPTIONSBUILDER_H 1



#include <string>

#include "clang/Basic/TargetOptions.h"



/**
 * A class for inline construction and initialization
 * of clang::TargetOptions instances.
 */
class TargetOptionsBuilder
{
    private:
        std::string Triple;
        std::string CPU;
        std::string ABI;
        std::string CXXABI;
        std::string LinkerVersion;
        std::vector<std::string> Features;

    public:
        inline static TargetOptionsBuilder& newToFree();
        inline static clang::TargetOptions constructor(std::string Triple = "", std::string CPU = "", std::string ABI = "", std::string CXXABI = "", std::string LinkerVersion = "", std::vector<std::string> Features = std::vector<std::string>());

        inline TargetOptionsBuilder& setTriple(std::string value);
        inline TargetOptionsBuilder& setCPU(std::string value);
        inline TargetOptionsBuilder& setABI(std::string value);
        inline TargetOptionsBuilder& setCXXABI(std::string value);
        inline TargetOptionsBuilder& setLinkerVersion(std::string value);
        inline TargetOptionsBuilder& addFeature(std::string value);

        inline clang::TargetOptions getTargetOptions();
        inline void free();
        inline clang::TargetOptions getTargetOptionsAndFree();
};



#include "TargetOptionsBuilder.inl"



#endif //! _TARGETOPTIONSBUILDER_H

