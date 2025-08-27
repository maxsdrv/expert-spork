## Debug
### Prepare
First install Vagrant & VirtualBox

#### Vagrant
###### Launch Dev VM Astra Linux
```bash
vagrant up
```
###### Launch Dev VM Ubuntu
Turn on VPN
```bash
OS=ubuntu vagrant up
```
Or first download https://portal.cloud.hashicorp.com/vagrant/discover/bento/ubuntu-24.04/versions/202404.26.0
and run
```bash
vagrant box add bento/ubuntu-24.04 virtualbox.box
rm virtualbox.box
```

###### Run an Ansible playbook
```bash
cd provision
. .venv/bin/activate
ansible-playbook update-services.yml -i inventory/development
```

### Deploy
###### Preparation
```bash
export REGISTRY_CREDS=<your_gitlab_login>:<personal_gitlab_token>

cd provision
# [see first](#prepare-ansible-controller)
. .venv/bin/activate
```

#### Ansible
###### Prepare ansible controller
```bash
# install Python 3 & virtualenv for your OS
# and then
cd provision
virtualenv .venv -p `which python3`
. .venv/bin/activate

pip install ansible-core==2.17.3
pip install -r requirements.txt
ansible-galaxy collection install -r requirements.yaml
```

##### Init host(s)
###### Vagrant VM
```bash
ansible-playbook init.yml -i inventory/development
```

###### Production
```bash
ansible-playbook init.yml -i inventory/production -e ansible_user=sudo_user --private-key=~/.ssh/admin.key -e ansible_host=<host_ip_or_dns>
```

##### Build local services
###### dds-control-* local services
```bash
devspace run use-vm-docker
devspace build
```

###### All local services
```bash
devspace run use-vm-docker
cd ../dds-local-env
devspace build
```

##### Deploy services

###### All local built services
```bash
ansible-playbook -i inventory/development update-services.yml
```

###### All services from registry
```bash
ansible-playbook -i inventory/development update-services.yml -e image_tag=develop -e online=yes
```

###### All services from registry except dds-control-* services
```bash
ansible-playbook -i inventory/development update-services.yml -e image_tag=develop -e online=yes --skip-tags control
```

###### dds-control-* Local built services
```bash
ansible-playbook -i inventory/development update-services.yml --tags control
```

###### Bootstrap script example
```bash
docker run --pull=always --privileged --pid=host registry.dev.mpksoft.ru/irz/tamerlan/dds-control/update:local ./bootstrap --version develop -- RADARIQ_DEVICES="localhost localhost1" TAMERLAN_DEVICES="localhost localhost"
docker run --pull=always --privileged --pid=host docker.support.3mx.ru/tamerlan/dds-control/update:develop ./bootstrap --version develop --oem-brand radariq
docker run registry.dev.mpksoft.ru/irz/tamerlan/dds-control/update:local ./bootstrap -h
```

##### Useful commands
```bash
# show tasks and tags map
ansible-playbook update-services.yml -i inventory/development --list-tasks

# show copied files diffs
ansible-playbook update-services.yml -i inventory/development --diff

# skip first tasks
ansible-playbook update-services.yml -i inventory/development --start-at-task "Deploy video service"
```

### General
#### Garbage collection Cron
Check logs
```bash
journalctl -u cron
```

### Preparation for debug with devspace

```bash
# [see first](#deploy-preparation)
cd ..
devspace run use-vm-docker
```

### Testing
#### Example: running test_time.py 
```bash
# [see first](#preparation-for-debug-with-devspace)
devspace run tests --url=http://192.168.56.101:8080/api/v1 tests/integration/test_time.py
```
