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
```bash
pdm run pytest --url=http://localhost:19080/api/v1
```

### Develop
#### Generating test API models on OpenAPI/AsyncAPI files update
```bash
pdm run gen_http
pdm run gen_ws
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
