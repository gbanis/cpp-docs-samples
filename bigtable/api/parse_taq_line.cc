// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "parse_taq_line.h"

#include <chrono>
#include <sstream>

namespace bigtable_api_samples {
Quote parse_taq_line(int lineno, std::string const& line) try {
  // Return the parsed line in this object ...
  Quote quote;

  // The data is in pipe separated fields, we extract them one at a time ...
  std::istringstream tokens(line);
  tokens.exceptions(std::ios::failbit);

  // Time: in HHMMSSNNNNNNNNN format (hours, minutes, seconds, nanoseconds).
  std::string tk;
  std::getline(tokens, tk, '|');  // fetch the timestamp as a string

  // ... parse the timestamp into a 64-bit long ...
  if (tk.length() != 15U) {
    std::ostringstream os;
    os << "timestamp field (" << tk << ") is not in HHMMSSNNNNNNNNN format";
    throw std::runtime_error(os.str());
  }
  int hh = std::stoi(tk.substr(0, 2));
  int mm = std::stoi(tk.substr(2, 2));
  int ss = std::stoi(tk.substr(4, 2));
  long long nnn = std::stoll(tk.substr(6));

  using namespace std::chrono;
  std::chrono::nanoseconds ns = duration_cast<nanoseconds>(hours(hh) + minutes(mm) +
							   seconds(ss) + nanoseconds(nnn));

  // ... store the number of nanoseconds in the timestamp ...
  quote.set_timestamp_ns(ns.count());

  // Exchange (ignored): a single character
  std::getline(tokens, tk, '|');  // ignore the exchange

  // Symbol: a string
  std::getline(tokens, tk, '|');  // fetch the symbol
  quote.set_ticker(std::move(tk));

  // Bid_Price: float
  std::getline(tokens, tk, '|');
  quote.set_bid_px(std::stod(tk));
  // Bid_Size: integer
  std::getline(tokens, tk, '|');
  quote.set_bid_qty(std::stoi(tk));
  // Offer_Price: float
  std::getline(tokens, tk, '|');
  quote.set_offer_px(std::stod(tk));
  // Offer_Size: integer
  std::getline(tokens, tk, '|');
  quote.set_offer_qty(std::stoi(tk));
  // ... the TAQ line has many other fields that we ignore in this demo ...

  return quote;
} catch (std::exception const& ex) {
  std::ostringstream os;
  os << ex.what() << " in line #" << lineno << " (" << line << ")";
  throw std::runtime_error(os.str());
}
} // namespace bigtable_api_samples
