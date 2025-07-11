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

// checks if the Band type satisfies the MappedNullable interface at compile time
var _ MappedNullable = &Band{}

// Band Band label and its activity status
type Band struct {
	Label string `json:"label"`
	Active bool `json:"active"`
}

type _Band Band

// NewBand instantiates a new Band object
// This constructor will assign default values to properties that have it defined,
// and makes sure properties required by API are set, but the set of arguments
// will change when the set of required properties is changed
func NewBand(label string, active bool) *Band {
	this := Band{}
	this.Label = label
	this.Active = active
	return &this
}

// NewBandWithDefaults instantiates a new Band object
// This constructor will only assign default values to properties that have it defined,
// but it doesn't guarantee that properties required by API are set
func NewBandWithDefaults() *Band {
	this := Band{}
	return &this
}

// GetLabel returns the Label field value
func (o *Band) GetLabel() string {
	if o == nil {
		var ret string
		return ret
	}

	return o.Label
}

// GetLabelOk returns a tuple with the Label field value
// and a boolean to check if the value has been set.
func (o *Band) GetLabelOk() (*string, bool) {
	if o == nil {
		return nil, false
	}
	return &o.Label, true
}

// SetLabel sets field value
func (o *Band) SetLabel(v string) {
	o.Label = v
}

// GetActive returns the Active field value
func (o *Band) GetActive() bool {
	if o == nil {
		var ret bool
		return ret
	}

	return o.Active
}

// GetActiveOk returns a tuple with the Active field value
// and a boolean to check if the value has been set.
func (o *Band) GetActiveOk() (*bool, bool) {
	if o == nil {
		return nil, false
	}
	return &o.Active, true
}

// SetActive sets field value
func (o *Band) SetActive(v bool) {
	o.Active = v
}

func (o Band) MarshalJSON() ([]byte, error) {
	toSerialize,err := o.ToMap()
	if err != nil {
		return []byte{}, err
	}
	return json.Marshal(toSerialize)
}

func (o Band) ToMap() (map[string]interface{}, error) {
	toSerialize := map[string]interface{}{}
	toSerialize["label"] = o.Label
	toSerialize["active"] = o.Active
	return toSerialize, nil
}

func (o *Band) UnmarshalJSON(data []byte) (err error) {
	// This validates that all required properties are included in the JSON object
	// by unmarshalling the object into a generic map with string keys and checking
	// that every required field exists as a key in the generic map.
	requiredProperties := []string{
		"label",
		"active",
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

	varBand := _Band{}

	decoder := json.NewDecoder(bytes.NewReader(data))
	decoder.DisallowUnknownFields()
	err = decoder.Decode(&varBand)

	if err != nil {
		return err
	}

	*o = Band(varBand)

	return err
}

type NullableBand struct {
	value *Band
	isSet bool
}

func (v NullableBand) Get() *Band {
	return v.value
}

func (v *NullableBand) Set(val *Band) {
	v.value = val
	v.isSet = true
}

func (v NullableBand) IsSet() bool {
	return v.isSet
}

func (v *NullableBand) Unset() {
	v.value = nil
	v.isSet = false
}

func NewNullableBand(val *Band) *NullableBand {
	return &NullableBand{value: val, isSet: true}
}

func (v NullableBand) MarshalJSON() ([]byte, error) {
	return json.Marshal(v.value)
}

func (v *NullableBand) UnmarshalJSON(src []byte) error {
	v.isSet = true
	return json.Unmarshal(src, &v.value)
}


