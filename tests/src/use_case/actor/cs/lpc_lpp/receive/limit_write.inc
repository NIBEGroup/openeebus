#include <string>

namespace cs_lpc_lpp_test {
namespace receive {

// Write of a single limit entry from the HEMS LoadControl client to the shared LoadControl server
inline std::string limit_write(int msg_counter, int limit_id, int64_t number) {
  return R"({
  "datagram": [
    {
      "header": [
        { "specificationVersion": "1.3.0" },
        {
          "addressSource": [
            { "device": "d:_n:HEMS_123456789" },
            { "entity": [1] },
            { "feature": 2 }
          ]
        },
        {
          "addressDestination": [
            { "device": "d:_n:HeatPump_123456789" },
            { "entity": [1] },
            { "feature": 2 }
          ]
        },
        { "msgCounter": )" + std::to_string(msg_counter) + R"( },
        { "cmdClassifier": "write" },
        { "ackRequest": true }
      ]
    },
    {
      "payload": [
        {
          "cmd": [
            [
              { "function": "loadControlLimitListData" },
              { "filter": [[{ "cmdControl": [{ "partial": [] }] }]] },
              {
                "loadControlLimitListData": [
                  {
                    "loadControlLimitData": [
                      [
                        { "limitId": )" + std::to_string(limit_id) + R"( },
                        { "isLimitActive": true },
                        { "value": [{ "number": )" + std::to_string(number) + R"( }, { "scale": 0 }] }
                      ]
                    ]
                  }
                ]
              }
            ]
          ]
        }
      ]
    }
  ]
})";
}

}  // namespace receive
}  // namespace cs_lpc_lpp_test
