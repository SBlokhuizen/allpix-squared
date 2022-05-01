/**
 * @file
 * @brief Implements the GENIE MC generator file reader module for primary particles
 *
 * @copyright Copyright (c) 2022 CERN and the Allpix Squared authors.
 * This software is distributed under the terms of the MIT License, copied verbatim in the file "LICENSE.md".
 * In applying this license, CERN does not waive the privileges and immunities granted to it by virtue of its status as an
 * Intergovernmental Organization or submit itself to any jurisdiction.
 * SPDX-License-Identifier: MIT
 */

#include "PrimariesReaderGenie.hpp"

#include "core/utils/log.h"

using namespace allpix;

PrimariesReaderGenie::PrimariesReaderGenie(const Configuration& config) {
    auto file_path = config.getPathWithExtension("file_name", "root", true);
    input_file_ = std::make_unique<TFile>(file_path.c_str(), "READ");
    if(!input_file_->IsOpen()) {
        throw InvalidValueError(config, "file_name", "could not open input file");
    }
    input_file_->cd();

    tree_reader_ = std::make_shared<TTreeReader>("tree", input_file_.get());
    if(tree_reader_->GetEntryStatus() == TTreeReader::kEntryNoTree) {
        throw InvalidValueError(config, "file_name", "could not open tree");
    }
    LOG(INFO) << "Initialized tree reader, found " << tree_reader_->GetEntries(false) << " entries";

    // Set up branch pointers
    create_tree_reader(event_, "idEvent");
    create_tree_reader(pdg_code_, "pdg");
    create_tree_reader(px_, "px");
    create_tree_reader(py_, "py");
    create_tree_reader(pz_, "pz");
    create_tree_reader(energy_, "E");

    // Advance to first entry of the tree:
    tree_reader_->Next();

    // Only after loading the first entry we can actually check the branch status:
    check_tree_reader(config, event_);
    check_tree_reader(config, pdg_code_);
    check_tree_reader(config, px_);
    check_tree_reader(config, py_);
    check_tree_reader(config, pz_);
    check_tree_reader(config, energy_);
}

template <typename T>
void PrimariesReaderGenie::create_tree_reader(std::shared_ptr<T>& branch_ptr, const std::string& name) {
    branch_ptr = std::make_shared<T>(*tree_reader_, name.c_str());
}

template <typename T>
void PrimariesReaderGenie::check_tree_reader(const Configuration& config, std::shared_ptr<T> branch_ptr) {
    if(branch_ptr->GetSetupStatus() < 0) {
        throw InvalidValueError(
            config, "file_name", "Could not read branch \"" + std::string(branch_ptr->GetBranchName()) + "\"");
    }
}

std::vector<PrimariesReader::Particle> PrimariesReaderGenie::getParticles() {
    return {};
}
