// Copyright (C) 2018-2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <fstream>

#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/text_format.h"
#include "graph_iterator_proto.hpp"
#include "openvino/frontend/exception.hpp"

namespace ov {
namespace frontend {
namespace tensorflow {

class GraphIteratorProtoTxt : public GraphIteratorProto {
public:
    template <typename T>
    GraphIteratorProtoTxt(const std::basic_string<T>& path) : GraphIteratorProto() {
        std::ifstream pbtxt_stream(path, std::ios::in);
        FRONT_END_GENERAL_CHECK(pbtxt_stream && pbtxt_stream.is_open(), "Model file does not exist");
        auto input_stream = std::make_shared<::google::protobuf::io::IstreamInputStream>(&pbtxt_stream);
        auto is_parsed = ::google::protobuf::TextFormat::Parse(input_stream.get(), m_graph_def.get());
        FRONT_END_GENERAL_CHECK(
            is_parsed,
            "[TensorFlow Frontend] Incorrect model or internal error: Model in text Protobuf format cannot be parsed.");

        initialize_decoders_and_library();
    }

    /// \brief Check if the input file is supported
    template <typename T>
    static bool is_supported(const std::basic_string<T>& path) {
        std::ifstream pbtxt_stream(path, std::ios::in);
        bool model_exists = (pbtxt_stream && pbtxt_stream.is_open());
        if (!model_exists) {
            return false;
        }
        auto input_stream = std::make_shared<::google::protobuf::io::IstreamInputStream>(&pbtxt_stream);
        if (!input_stream) {
            return false;
        }
        auto graph_def = std::make_shared<::tensorflow::GraphDef>();
        auto is_parsed = ::google::protobuf::TextFormat::Parse(input_stream.get(), graph_def.get());
        return is_parsed;
    }
};
}  // namespace tensorflow
}  // namespace frontend
}  // namespace ov
