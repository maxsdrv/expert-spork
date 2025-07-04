/*
DDS Provider REST API

REST API of `dds-target-provider`.

API version: 2.3.0
*/

// Code generated by OpenAPI Generator (https://openapi-generator.tech); DO NOT EDIT.

package dss_target_service

import (
	"encoding/json"
	"bytes"
	"fmt"
)

// checks if the TargetList type satisfies the MappedNullable interface at compile time
var _ MappedNullable = &TargetList{}

// TargetList struct for TargetList
type TargetList struct {
	Targets []TargetData `json:"targets"`
}

type _TargetList TargetList

// NewTargetList instantiates a new TargetList object
// This constructor will assign default values to properties that have it defined,
// and makes sure properties required by API are set, but the set of arguments
// will change when the set of required properties is changed
func NewTargetList(targets []TargetData) *TargetList {
	this := TargetList{}
	this.Targets = targets
	return &this
}

// NewTargetListWithDefaults instantiates a new TargetList object
// This constructor will only assign default values to properties that have it defined,
// but it doesn't guarantee that properties required by API are set
func NewTargetListWithDefaults() *TargetList {
	this := TargetList{}
	return &this
}

// GetTargets returns the Targets field value
func (o *TargetList) GetTargets() []TargetData {
	if o == nil {
		var ret []TargetData
		return ret
	}

	return o.Targets
}

// GetTargetsOk returns a tuple with the Targets field value
// and a boolean to check if the value has been set.
func (o *TargetList) GetTargetsOk() ([]TargetData, bool) {
	if o == nil {
		return nil, false
	}
	return o.Targets, true
}

// SetTargets sets field value
func (o *TargetList) SetTargets(v []TargetData) {
	o.Targets = v
}

func (o TargetList) MarshalJSON() ([]byte, error) {
	toSerialize,err := o.ToMap()
	if err != nil {
		return []byte{}, err
	}
	return json.Marshal(toSerialize)
}

func (o TargetList) ToMap() (map[string]interface{}, error) {
	toSerialize := map[string]interface{}{}
	toSerialize["targets"] = o.Targets
	return toSerialize, nil
}

func (o *TargetList) UnmarshalJSON(data []byte) (err error) {
	// This validates that all required properties are included in the JSON object
	// by unmarshalling the object into a generic map with string keys and checking
	// that every required field exists as a key in the generic map.
	requiredProperties := []string{
		"targets",
	}

	allProperties := make(map[string]interface{})

	err = json.Unmarshal(data, &allProperties)

	if err != nil {
		return err;
	}

	for _, requiredProperty := range(requiredProperties) {
		if _, exists := allProperties[requiredProperty]; !exists {
			return fmt.Errorf("no value given for required property %v", requiredProperty)
		}
	}

	varTargetList := _TargetList{}

	decoder := json.NewDecoder(bytes.NewReader(data))
	decoder.DisallowUnknownFields()
	err = decoder.Decode(&varTargetList)

	if err != nil {
		return err
	}

	*o = TargetList(varTargetList)

	return err
}

type NullableTargetList struct {
	value *TargetList
	isSet bool
}

func (v NullableTargetList) Get() *TargetList {
	return v.value
}

func (v *NullableTargetList) Set(val *TargetList) {
	v.value = val
	v.isSet = true
}

func (v NullableTargetList) IsSet() bool {
	return v.isSet
}

func (v *NullableTargetList) Unset() {
	v.value = nil
	v.isSet = false
}

func NewNullableTargetList(val *TargetList) *NullableTargetList {
	return &NullableTargetList{value: val, isSet: true}
}

func (v NullableTargetList) MarshalJSON() ([]byte, error) {
	return json.Marshal(v.value)
}

func (v *NullableTargetList) UnmarshalJSON(src []byte) error {
	v.isSet = true
	return json.Unmarshal(src, &v.value)
}


