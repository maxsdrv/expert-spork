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

// checks if the GeoDirection type satisfies the MappedNullable interface at compile time
var _ MappedNullable = &GeoDirection{}

// GeoDirection struct for GeoDirection
type GeoDirection struct {
	// Bearing on object in angular degrees
	Bearing float64 `json:"bearing"`
	// Distance to object in meters
	Distance float64 `json:"distance"`
	// Object elevation in angular degrees
	Elevation *float64 `json:"elevation,omitempty"`
}

type _GeoDirection GeoDirection

// NewGeoDirection instantiates a new GeoDirection object
// This constructor will assign default values to properties that have it defined,
// and makes sure properties required by API are set, but the set of arguments
// will change when the set of required properties is changed
func NewGeoDirection(bearing float64, distance float64) *GeoDirection {
	this := GeoDirection{}
	this.Bearing = bearing
	this.Distance = distance
	return &this
}

// NewGeoDirectionWithDefaults instantiates a new GeoDirection object
// This constructor will only assign default values to properties that have it defined,
// but it doesn't guarantee that properties required by API are set
func NewGeoDirectionWithDefaults() *GeoDirection {
	this := GeoDirection{}
	return &this
}

// GetBearing returns the Bearing field value
func (o *GeoDirection) GetBearing() float64 {
	if o == nil {
		var ret float64
		return ret
	}

	return o.Bearing
}

// GetBearingOk returns a tuple with the Bearing field value
// and a boolean to check if the value has been set.
func (o *GeoDirection) GetBearingOk() (*float64, bool) {
	if o == nil {
		return nil, false
	}
	return &o.Bearing, true
}

// SetBearing sets field value
func (o *GeoDirection) SetBearing(v float64) {
	o.Bearing = v
}

// GetDistance returns the Distance field value
func (o *GeoDirection) GetDistance() float64 {
	if o == nil {
		var ret float64
		return ret
	}

	return o.Distance
}

// GetDistanceOk returns a tuple with the Distance field value
// and a boolean to check if the value has been set.
func (o *GeoDirection) GetDistanceOk() (*float64, bool) {
	if o == nil {
		return nil, false
	}
	return &o.Distance, true
}

// SetDistance sets field value
func (o *GeoDirection) SetDistance(v float64) {
	o.Distance = v
}

// GetElevation returns the Elevation field value if set, zero value otherwise.
func (o *GeoDirection) GetElevation() float64 {
	if o == nil || IsNil(o.Elevation) {
		var ret float64
		return ret
	}
	return *o.Elevation
}

// GetElevationOk returns a tuple with the Elevation field value if set, nil otherwise
// and a boolean to check if the value has been set.
func (o *GeoDirection) GetElevationOk() (*float64, bool) {
	if o == nil || IsNil(o.Elevation) {
		return nil, false
	}
	return o.Elevation, true
}

// HasElevation returns a boolean if a field has been set.
func (o *GeoDirection) HasElevation() bool {
	if o != nil && !IsNil(o.Elevation) {
		return true
	}

	return false
}

// SetElevation gets a reference to the given float64 and assigns it to the Elevation field.
func (o *GeoDirection) SetElevation(v float64) {
	o.Elevation = &v
}

func (o GeoDirection) MarshalJSON() ([]byte, error) {
	toSerialize,err := o.ToMap()
	if err != nil {
		return []byte{}, err
	}
	return json.Marshal(toSerialize)
}

func (o GeoDirection) ToMap() (map[string]interface{}, error) {
	toSerialize := map[string]interface{}{}
	toSerialize["bearing"] = o.Bearing
	toSerialize["distance"] = o.Distance
	if !IsNil(o.Elevation) {
		toSerialize["elevation"] = o.Elevation
	}
	return toSerialize, nil
}

func (o *GeoDirection) UnmarshalJSON(data []byte) (err error) {
	// This validates that all required properties are included in the JSON object
	// by unmarshalling the object into a generic map with string keys and checking
	// that every required field exists as a key in the generic map.
	requiredProperties := []string{
		"bearing",
		"distance",
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

	varGeoDirection := _GeoDirection{}

	decoder := json.NewDecoder(bytes.NewReader(data))
	decoder.DisallowUnknownFields()
	err = decoder.Decode(&varGeoDirection)

	if err != nil {
		return err
	}

	*o = GeoDirection(varGeoDirection)

	return err
}

type NullableGeoDirection struct {
	value *GeoDirection
	isSet bool
}

func (v NullableGeoDirection) Get() *GeoDirection {
	return v.value
}

func (v *NullableGeoDirection) Set(val *GeoDirection) {
	v.value = val
	v.isSet = true
}

func (v NullableGeoDirection) IsSet() bool {
	return v.isSet
}

func (v *NullableGeoDirection) Unset() {
	v.value = nil
	v.isSet = false
}

func NewNullableGeoDirection(val *GeoDirection) *NullableGeoDirection {
	return &NullableGeoDirection{value: val, isSet: true}
}

func (v NullableGeoDirection) MarshalJSON() ([]byte, error) {
	return json.Marshal(v.value)
}

func (v *NullableGeoDirection) UnmarshalJSON(src []byte) error {
	v.isSet = true
	return json.Unmarshal(src, &v.value)
}


