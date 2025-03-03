/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

#include <memory>

#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"

#include "boost/graph/copy.hpp"
#include "boost/graph/depth_first_search.hpp"
#include "boost/graph/graph_utility.hpp"
#include "boost/graph/graphviz.hpp"

#include "phasar/DB/ProjectIRDB.h"
#include "phasar/PhasarLLVM/ControlFlow/LLVMBasedBackwardICFG.h"
#include "phasar/PhasarLLVM/ControlFlow/LLVMBasedICFG.h"
#include "phasar/PhasarLLVM/TypeHierarchy/LLVMTypeHierarchy.h"
#include "phasar/Utils/LLVMShorthands.h"
#include "phasar/Utils/Logger.h"
#include "phasar/Utils/PAMM.h"
#include "phasar/Utils/Utilities.h"

using namespace psr;
using namespace std;
namespace psr {

LLVMBasedBackwardsICFG::LLVMBasedBackwardsICFG(LLVMBasedICFG &ICFG)
    : ForwardICFG(ICFG) {
  auto CgCopy = ForwardICFG.CallGraph;
  boost::copy_graph(boost::make_reverse_graph(CgCopy), ForwardICFG.CallGraph);
}

LLVMBasedBackwardsICFG::LLVMBasedBackwardsICFG(
    ProjectIRDB &IRDB, CallGraphAnalysisType CGType,
    const std::set<std::string> &EntryPoints, LLVMTypeHierarchy *TH,
    LLVMPointsToInfo *PT, Soundness S)
    : ForwardICFG(IRDB, CGType, EntryPoints, TH, PT, S) {
  auto CgCopy = ForwardICFG.CallGraph;
  boost::copy_graph(boost::make_reverse_graph(CgCopy), ForwardICFG.CallGraph);
}

bool LLVMBasedBackwardsICFG::isIndirectFunctionCall(
    const llvm::Instruction *Stmt) const {
  return ForwardICFG.isIndirectFunctionCall(Stmt);
}

bool LLVMBasedBackwardsICFG::isVirtualFunctionCall(
    const llvm::Instruction *Stmt) const {
  return ForwardICFG.isVirtualFunctionCall(Stmt);
}

std::set<const llvm::Function *>
LLVMBasedBackwardsICFG::getAllFunctions() const {
  return ForwardICFG.getAllFunctions();
}

const llvm::Function *
LLVMBasedBackwardsICFG::getFunction(const std::string &Fun) const {
  return ForwardICFG.getFunction(Fun);
}

std::set<const llvm::Function *>
LLVMBasedBackwardsICFG::getCalleesOfCallAt(const llvm::Instruction *N) const {
  return ForwardICFG.getCalleesOfCallAt(N);
}

std::set<const llvm::Instruction *>
LLVMBasedBackwardsICFG::getCallersOf(const llvm::Function *M) const {
  return ForwardICFG.getCallersOf(M);
}

std::set<const llvm::Instruction *>
LLVMBasedBackwardsICFG::getCallsFromWithin(const llvm::Function *M) const {
  return ForwardICFG.getCallsFromWithin(M);
}

std::set<const llvm::Instruction *>
LLVMBasedBackwardsICFG::getReturnSitesOfCallAt(
    const llvm::Instruction *N) const {
  std::set<const llvm::Instruction *> ReturnSites;
  if (const auto *Call = llvm::dyn_cast<llvm::CallInst>(N)) {
    for (const auto *Succ : this->getSuccsOf(Call)) {
      ReturnSites.insert(Succ);
    }
  }
  if (const auto *Invoke = llvm::dyn_cast<llvm::InvokeInst>(N)) {
    ReturnSites.insert(&Invoke->getNormalDest()->back());
    ReturnSites.insert(&Invoke->getUnwindDest()->back());
  }
  return ReturnSites;
}

std::vector<const llvm::Function *>
LLVMBasedBackwardsICFG::getGlobalCtors() const {
  return ForwardICFG.getGlobalCtors();
}

std::vector<const llvm::Function *>
LLVMBasedBackwardsICFG::getGlobalDtors() const {
  return ForwardICFG.getGlobalDtors();
}

std::set<const llvm::Instruction *>
LLVMBasedBackwardsICFG::allNonCallStartNodes() const {
  return ForwardICFG.allNonCallStartNodes();
}

void LLVMBasedBackwardsICFG::mergeWith(const LLVMBasedBackwardsICFG &Other) {
  ForwardICFG.mergeWith(Other.ForwardICFG);
}

void LLVMBasedBackwardsICFG::print(std::ostream &OS) const {
  ForwardICFG.print(OS);
}

void LLVMBasedBackwardsICFG::printAsDot(std::ostream &OS) const {
  ForwardICFG.printAsDot(OS);
}

nlohmann::json LLVMBasedBackwardsICFG::getAsJson() const {
  return ForwardICFG.getAsJson();
}

unsigned LLVMBasedBackwardsICFG::getNumOfVertices() {
  return ForwardICFG.getNumOfVertices();
}

unsigned LLVMBasedBackwardsICFG::getNumOfEdges() {
  return ForwardICFG.getNumOfEdges();
}

std::vector<const llvm::Function *>
LLVMBasedBackwardsICFG::getDependencyOrderedFunctions() {
  return ForwardICFG.getDependencyOrderedFunctions();
}

} // namespace psr
