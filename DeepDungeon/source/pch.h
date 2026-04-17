#pragma once

// Standard library — stable, expensive to parse, included here once via PCH.
#include <random>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <filesystem>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <cstdint>
#include <cstddef>

// Third-party header-only libraries — heavy template machinery, compiled once via PCH.
#include <nlohmann/json.hpp>
#include <pcg/pcg_random.hpp>
#include <magic_enum/magic_enum.hpp>
#include <YYToolkit/YYTK_Shared.hpp>
