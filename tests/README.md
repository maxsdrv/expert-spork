# DSS API Tests

## Tests
First you need installing [PDM](https://pdm-project.org/en/latest/#other-installation-methods) Python package manager
### Prepare python environment
```bash
pdm run deps
```

### Run
Example for local-dev environment
> DSS Service and it's dependencies (radar-iq or others) needs to be deployed (run) beforehand
####Single test
> From project root:
```bash
devspace run tests --url http://192.168.56.101:8080/api/v1 tests/integration/test_config.py::test_config_system
```
####All tests  (Workaround until fixed)
```bash
devspace run tests --url http://192.168.56.101:8080/api/v1 tests/integration
```

### Develop
#### Generating test API models on OpenAPI/AsyncAPI files update
```bash
> From project root:
devspace run tests_update_models
```

[//]: # (#### Coverage test )

[//]: # (```bash)

[//]: # (devspace build)

[//]: # (devspace run-pipeline cov-test)

[//]: # (```)



[//]: # ()
[//]: # (#### Update tests dependencies lock)

[//]: # (```bash)

[//]: # (pdm run lock)

[//]: # (```)
