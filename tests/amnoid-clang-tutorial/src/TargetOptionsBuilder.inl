#include "TargetOptionsBuilder.h"

TargetOptionsBuilder& TargetOptionsBuilder::newToFree()
{
    return *(new TargetOptionsBuilder());
}

clang::TargetOptions TargetOptionsBuilder::constructor(std::string Triple, std::string CPU, std::string ABI, std::string CXXABI, std::string LinkerVersion, std::vector<std::string> Features)
{
    clang::TargetOptions rtn;
    rtn.Triple = Triple;
    rtn.CPU = CPU;
    rtn.ABI = ABI;
    rtn.CXXABI = CXXABI;
    rtn.LinkerVersion = LinkerVersion;
    rtn.Features = Features;
    return rtn;
}


TargetOptionsBuilder& TargetOptionsBuilder::setTriple(std::string value)
{
    Triple = value;
    return *this;
}

TargetOptionsBuilder& TargetOptionsBuilder::setCPU(std::string value)
{
    CPU = value;
    return *this;
}

TargetOptionsBuilder& TargetOptionsBuilder::setABI(std::string value)
{
    ABI = value;
    return *this;
}

TargetOptionsBuilder& TargetOptionsBuilder::setCXXABI(std::string value)
{
    CXXABI = value;
    return *this;
}

TargetOptionsBuilder& TargetOptionsBuilder::setLinkerVersion(std::string value)
{
    LinkerVersion = value;
    return *this;
}

TargetOptionsBuilder& TargetOptionsBuilder::addFeature(std::string value)
{
    Features.push_back(value);
    return *this;
}


clang::TargetOptions TargetOptionsBuilder::getTargetOptions()
{
    clang::TargetOptions rtn;
    rtn.Triple = Triple;
    rtn.CPU = CPU;
    rtn.ABI = ABI;
    rtn.CXXABI = CXXABI;
    rtn.LinkerVersion = LinkerVersion;
    rtn.Features = Features;
    return rtn;
}

void TargetOptionsBuilder::free()
{
    delete this;
}

clang::TargetOptions TargetOptionsBuilder::getTargetOptionsAndFree()
{
    clang::TargetOptions rtn;
    rtn.Triple = Triple;
    rtn.CPU = CPU;
    rtn.ABI = ABI;
    rtn.CXXABI = CXXABI;
    rtn.LinkerVersion = LinkerVersion;
    rtn.Features = Features;
    free();
    return rtn;
}

