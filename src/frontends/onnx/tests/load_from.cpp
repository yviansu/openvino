// Copyright (C) 2018-2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
#include "load_from.hpp"

#include <gtest/gtest.h>

#include <fstream>

#include "common_test_utils/test_assertions.hpp"
#include "onnx_utils.hpp"
#include "utils.hpp"

using namespace ov::frontend;

using ONNXLoadTest = FrontEndLoadFromTest;

static LoadFromFEParam getTestData() {
    LoadFromFEParam res;
    res.m_frontEndName = ONNX_FE;
    res.m_modelsPath = std::string(TEST_ONNX_MODELS_DIRNAME);
    res.m_file = "external_data/external_data.onnx";
    res.m_stream = "add_abc.onnx";
    return res;
}

TEST_P(FrontEndLoadFromTest, testLoadFromStreamAndPassPath) {
    const auto path = ov::util::path_join(
        {ov::test::utils::getExecutableDirectory(), TEST_ONNX_MODELS_DIRNAME, "external_data/external_data.onnx"});
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    ASSERT_TRUE(ifs.is_open()) << "Could not open an ifstream for the model path: " << path;
    std::istream* is = &ifs;
    std::vector<std::string> frontends;
    FrontEnd::Ptr fe;
    ASSERT_NO_THROW(frontends = m_fem.get_available_front_ends());
    ASSERT_NO_THROW(m_frontEnd = m_fem.load_by_model(is)) << "Could not create the ONNX FE using the istream object";
    ASSERT_NE(m_frontEnd, nullptr);

    ASSERT_NO_THROW(m_inputModel = m_frontEnd->load(is, path)) << "Could not load the model";
    ASSERT_NE(m_inputModel, nullptr);

    std::shared_ptr<ov::Model> function;
    ASSERT_NO_THROW(function = m_frontEnd->convert(m_inputModel)) << "Could not convert the model to OV representation";
    ASSERT_NE(function, nullptr);
}

TEST_P(FrontEndLoadFromTest, load_model_not_exists_at_path) {
    const auto model_name = "not_existing_model";
    auto error_msg = std::string("Could not open the file: ");
    auto model_file_path = FrontEndTestUtils::make_model_path(model_name);
    error_msg += '"' + model_file_path + '"';

    auto fem = ov::frontend::FrontEndManager();
    auto fe = fem.load_by_framework("onnx");

    OV_EXPECT_THROW(fe->supported({model_file_path}), ov::Exception, testing::HasSubstr(error_msg));
    OV_EXPECT_THROW(fe->load(model_file_path), ov::Exception, testing::HasSubstr(error_msg));
}

INSTANTIATE_TEST_SUITE_P(ONNXLoadTest,
                         FrontEndLoadFromTest,
                         ::testing::Values(getTestData()),
                         FrontEndLoadFromTest::getTestCaseName);
