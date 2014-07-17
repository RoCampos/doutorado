#include "strategy_make_tree.h"

TreeAlgorithm::TreeAlgorithm (TreeAlgorithmInterface & param) {
    m_strategy = &param;
}