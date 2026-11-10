# Use case unit tests
These use case abstraction-level unit tests are not regular unit tests, but rather a sandbox
where the SPINE device is fed with specific datagrams and the printed outgoing messages are checked; only basic checks are performed.

By default, logs are disabled in `UseCaseTestFixture()` with the `log_messages = kUseCaseLogMessagesDisabled`.
To enable the logging e.g. in Compressor OHPCF unit test, set the `kUseCaseLogMessagesEnabled` explicitly:
```cpp
class CompressorOhpcfTestFixture : public UseCaseTestFixture {
 public:
  CompressorOhpcfTestFixture()
      : UseCaseTestFixture("HeatGenerationSystem", "HeatPump", "123456789", kUseCaseLogMessagesEnabled) {};
  //...
}
```
