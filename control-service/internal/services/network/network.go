package network

import (
	"context"
	"errors"
	"strconv"

	"gopkg.in/yaml.v3"

	"github.com/opticoder/ctx-log/go/ctx_log"

	"dds-control-api/internal/generated/api"
)

type Nameservers struct {
	Addresses []string `yaml:"addresses"`
}

type Route struct {
	To  string `yaml:"to"`
	Via string `yaml:"via"`
}

func findNode(nodeTag string, node *yaml.Node) *yaml.Node {
	nodeFound := false
	for i, node := range node.Content {
		if nodeFound && i%2 == 1 {
			return node
		}
		if node.Value == nodeTag && i%2 == 0 {
			nodeFound = true
		}
		if len(node.Content) > 0 {
			if childNode := findNode(nodeTag, node); childNode != nil {
				return childNode
			}
		}
	}
	return nil
}

func addSubnode(parentNode *yaml.Node, tag string, value interface{}) {
	var tagNode yaml.Node
	var valueNode yaml.Node
	tagNode.Encode(tag)
	valueNode.Encode(value)
	parentNode.Content = append(parentNode.Content, &tagNode)
	parentNode.Content = append(parentNode.Content, &valueNode)
}

func removeSubnode(parentNode *yaml.Node, tag string) {
	for i, node := range parentNode.Content {
		if node.Value == tag && i%2 == 0 {
			parentNode.Content = append(parentNode.Content[:i], parentNode.Content[i+2:]...)
			return
		}
	}
}

func ModifyNetplanConfig(ctx context.Context, fileContent string, netInterface string, settings api.NetworkSettings) (string, error) {
	logging := ctx_log.GetLogger(nil).WithCtxFields(ctx)
	logger := logging.WithCtxFields(ctx)

	var netplanSettings yaml.Node
	if err := yaml.Unmarshal([]byte(fileContent), &netplanSettings); err != nil {
		logger.WithError(err).Error("Failed to parse netplan config file")
		return "", err
	}
	netInterfaceNode := findNode(netInterface, &netplanSettings)
	if netInterfaceNode != nil {
		// dhcp4
		dhcp4Node := findNode("dhcp4", netInterfaceNode)
		if dhcp4Node != nil {
			dhcp4Node.Encode("no")
		} else {
			addSubnode(netInterfaceNode, "dhcp4", "no")
		}

		// addresses
		var addresses []string
		addresses = append(addresses, settings.Address+"/"+strconv.Itoa(int(settings.Prefix)))
		addressesNode := findNode("addresses", netInterfaceNode)
		if addressesNode != nil {
			addressesNode.Encode(addresses)
		} else {
			addSubnode(netInterfaceNode, "addresses", addresses)
		}

		// routes
		if settings.Gateway != "" {
			var routes []Route
			routes = append(routes, Route{"default", settings.Gateway})
			routesNode := findNode("routes", netInterfaceNode)
			if routesNode != nil {
				routesNode.Encode(routes)
			} else {
				addSubnode(netInterfaceNode, "routes", routes)
			}
		} else {
			routesNode := findNode("routes", netInterfaceNode)
			if routesNode != nil {
				removeSubnode(netInterfaceNode, "routes")
			}
		}

		// nameservers
		nameserversNode := findNode("nameservers", netInterfaceNode)
		if nameserversNode != nil {
			nameserversAddressesNode := findNode("addresses", nameserversNode)
			if nameserversAddressesNode != nil {
				nameserversAddressesNode.Encode(settings.DnsNameservers)
			} else {
				addSubnode(nameserversNode, "addresses", settings.DnsNameservers)
			}
		} else {
			addSubnode(netInterfaceNode, "nameservers", Nameservers{settings.DnsNameservers})
		}
	} else {
		return "", errors.New("failed to find net interface node in the netplan yaml: " + netInterface)
	}

	netplanSettingsYaml, err := yaml.Marshal(&netplanSettings)
	if err != nil {
		logger.WithError(err).Error("Failed to marshal netplan settings to yaml")
		return "", err
	}
	return string(netplanSettingsYaml), nil
}
