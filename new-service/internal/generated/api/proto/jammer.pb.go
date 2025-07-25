// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.36.6
// 	protoc        (unknown)
// source: jammer.proto

package apiv1

import (
	_ "buf.build/gen/go/bufbuild/protovalidate/protocolbuffers/go/buf/validate"
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
	emptypb "google.golang.org/protobuf/types/known/emptypb"
	timestamppb "google.golang.org/protobuf/types/known/timestamppb"
	reflect "reflect"
	sync "sync"
	unsafe "unsafe"
)

const (
	// Verify that this generated code is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(20 - protoimpl.MinVersion)
	// Verify that runtime/protoimpl is sufficiently up-to-date.
	_ = protoimpl.EnforceVersion(protoimpl.MaxVersion - 20)
)

type JammerInfo struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	JammerId      *string                `protobuf:"bytes,1,opt,name=jammer_id,json=jammerId,proto3,oneof" json:"jammer_id,omitempty"`
	Model         *string                `protobuf:"bytes,2,opt,name=model,proto3,oneof" json:"model,omitempty"`
	Serial        *string                `protobuf:"bytes,3,opt,name=serial,proto3,oneof" json:"serial,omitempty"`
	SwVersion     *string                `protobuf:"bytes,4,opt,name=sw_version,json=swVersion,proto3,oneof" json:"sw_version,omitempty"`
	SensorId      *string                `protobuf:"bytes,10,opt,name=sensor_id,json=sensorId,proto3,oneof" json:"sensor_id,omitempty"`
	GroupId       *string                `protobuf:"bytes,15,opt,name=group_id,json=groupId,proto3,oneof" json:"group_id,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *JammerInfo) Reset() {
	*x = JammerInfo{}
	mi := &file_jammer_proto_msgTypes[0]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *JammerInfo) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JammerInfo) ProtoMessage() {}

func (x *JammerInfo) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[0]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JammerInfo.ProtoReflect.Descriptor instead.
func (*JammerInfo) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{0}
}

func (x *JammerInfo) GetJammerId() string {
	if x != nil && x.JammerId != nil {
		return *x.JammerId
	}
	return ""
}

func (x *JammerInfo) GetModel() string {
	if x != nil && x.Model != nil {
		return *x.Model
	}
	return ""
}

func (x *JammerInfo) GetSerial() string {
	if x != nil && x.Serial != nil {
		return *x.Serial
	}
	return ""
}

func (x *JammerInfo) GetSwVersion() string {
	if x != nil && x.SwVersion != nil {
		return *x.SwVersion
	}
	return ""
}

func (x *JammerInfo) GetSensorId() string {
	if x != nil && x.SensorId != nil {
		return *x.SensorId
	}
	return ""
}

func (x *JammerInfo) GetGroupId() string {
	if x != nil && x.GroupId != nil {
		return *x.GroupId
	}
	return ""
}

type JammersResponse struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	JammerInfos   []*JammerInfo          `protobuf:"bytes,1,rep,name=jammer_infos,json=jammerInfos,proto3" json:"jammer_infos,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *JammersResponse) Reset() {
	*x = JammersResponse{}
	mi := &file_jammer_proto_msgTypes[1]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *JammersResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JammersResponse) ProtoMessage() {}

func (x *JammersResponse) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[1]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JammersResponse.ProtoReflect.Descriptor instead.
func (*JammersResponse) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{1}
}

func (x *JammersResponse) GetJammerInfos() []*JammerInfo {
	if x != nil {
		return x.JammerInfos
	}
	return nil
}

type JammerInfoRequest struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	JammerId      *string                `protobuf:"bytes,1,opt,name=jammer_id,json=jammerId,proto3,oneof" json:"jammer_id,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *JammerInfoRequest) Reset() {
	*x = JammerInfoRequest{}
	mi := &file_jammer_proto_msgTypes[2]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *JammerInfoRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JammerInfoRequest) ProtoMessage() {}

func (x *JammerInfoRequest) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[2]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JammerInfoRequest.ProtoReflect.Descriptor instead.
func (*JammerInfoRequest) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{2}
}

func (x *JammerInfoRequest) GetJammerId() string {
	if x != nil && x.JammerId != nil {
		return *x.JammerId
	}
	return ""
}

type JammerInfoDynamicRequest struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	JammerId      *string                `protobuf:"bytes,1,opt,name=jammer_id,json=jammerId,proto3,oneof" json:"jammer_id,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *JammerInfoDynamicRequest) Reset() {
	*x = JammerInfoDynamicRequest{}
	mi := &file_jammer_proto_msgTypes[3]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *JammerInfoDynamicRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JammerInfoDynamicRequest) ProtoMessage() {}

func (x *JammerInfoDynamicRequest) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[3]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JammerInfoDynamicRequest.ProtoReflect.Descriptor instead.
func (*JammerInfoDynamicRequest) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{3}
}

func (x *JammerInfoDynamicRequest) GetJammerId() string {
	if x != nil && x.JammerId != nil {
		return *x.JammerId
	}
	return ""
}

type JammerInfoDynamicResponse struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	Disabled      *bool                  `protobuf:"varint,5,opt,name=disabled,proto3,oneof" json:"disabled,omitempty"`
	State         *string                `protobuf:"bytes,6,opt,name=state,proto3,oneof" json:"state,omitempty"`
	Position      *GeoPosition           `protobuf:"bytes,7,opt,name=position,proto3,oneof" json:"position,omitempty"`
	PositionMode  *GeoPositionMode       `protobuf:"varint,8,opt,name=position_mode,json=positionMode,proto3,enum=api.v1.GeoPositionMode,oneof" json:"position_mode,omitempty"`
	Workzone      []*WorkzoneSector      `protobuf:"bytes,9,rep,name=workzone,proto3" json:"workzone,omitempty"`
	Bands         []*Band                `protobuf:"bytes,11,rep,name=bands,proto3" json:"bands,omitempty"`
	BandOptions   []*BandOption          `protobuf:"bytes,12,rep,name=band_options,json=bandOptions,proto3" json:"band_options,omitempty"`
	HwInfo        *HwInfo                `protobuf:"bytes,13,opt,name=hw_info,json=hwInfo,proto3,oneof" json:"hw_info,omitempty"`
	TimeoutStatus *JammerTimeoutStatus   `protobuf:"bytes,14,opt,name=timeout_status,json=timeoutStatus,proto3,oneof" json:"timeout_status,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *JammerInfoDynamicResponse) Reset() {
	*x = JammerInfoDynamicResponse{}
	mi := &file_jammer_proto_msgTypes[4]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *JammerInfoDynamicResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JammerInfoDynamicResponse) ProtoMessage() {}

func (x *JammerInfoDynamicResponse) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[4]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JammerInfoDynamicResponse.ProtoReflect.Descriptor instead.
func (*JammerInfoDynamicResponse) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{4}
}

func (x *JammerInfoDynamicResponse) GetDisabled() bool {
	if x != nil && x.Disabled != nil {
		return *x.Disabled
	}
	return false
}

func (x *JammerInfoDynamicResponse) GetState() string {
	if x != nil && x.State != nil {
		return *x.State
	}
	return ""
}

func (x *JammerInfoDynamicResponse) GetPosition() *GeoPosition {
	if x != nil {
		return x.Position
	}
	return nil
}

func (x *JammerInfoDynamicResponse) GetPositionMode() GeoPositionMode {
	if x != nil && x.PositionMode != nil {
		return *x.PositionMode
	}
	return GeoPositionMode_GEO_AUTO
}

func (x *JammerInfoDynamicResponse) GetWorkzone() []*WorkzoneSector {
	if x != nil {
		return x.Workzone
	}
	return nil
}

func (x *JammerInfoDynamicResponse) GetBands() []*Band {
	if x != nil {
		return x.Bands
	}
	return nil
}

func (x *JammerInfoDynamicResponse) GetBandOptions() []*BandOption {
	if x != nil {
		return x.BandOptions
	}
	return nil
}

func (x *JammerInfoDynamicResponse) GetHwInfo() *HwInfo {
	if x != nil {
		return x.HwInfo
	}
	return nil
}

func (x *JammerInfoDynamicResponse) GetTimeoutStatus() *JammerTimeoutStatus {
	if x != nil {
		return x.TimeoutStatus
	}
	return nil
}

type GroupsResponse struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	JammerGroups  []*JammerGroup         `protobuf:"bytes,1,rep,name=jammer_groups,json=jammerGroups,proto3" json:"jammer_groups,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *GroupsResponse) Reset() {
	*x = GroupsResponse{}
	mi := &file_jammer_proto_msgTypes[5]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *GroupsResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*GroupsResponse) ProtoMessage() {}

func (x *GroupsResponse) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[5]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use GroupsResponse.ProtoReflect.Descriptor instead.
func (*GroupsResponse) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{5}
}

func (x *GroupsResponse) GetJammerGroups() []*JammerGroup {
	if x != nil {
		return x.JammerGroups
	}
	return nil
}

type JammerGroup struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	GroupId       *string                `protobuf:"bytes,1,opt,name=group_id,json=groupId,proto3,oneof" json:"group_id,omitempty"`
	Name          *string                `protobuf:"bytes,2,opt,name=name,proto3,oneof" json:"name,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *JammerGroup) Reset() {
	*x = JammerGroup{}
	mi := &file_jammer_proto_msgTypes[6]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *JammerGroup) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JammerGroup) ProtoMessage() {}

func (x *JammerGroup) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[6]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JammerGroup.ProtoReflect.Descriptor instead.
func (*JammerGroup) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{6}
}

func (x *JammerGroup) GetGroupId() string {
	if x != nil && x.GroupId != nil {
		return *x.GroupId
	}
	return ""
}

func (x *JammerGroup) GetName() string {
	if x != nil && x.Name != nil {
		return *x.Name
	}
	return ""
}

// MOVE ws_reply_id inside implementation: provider can wait for ws result then hand it over to UI
type SetJammerBandsRequest struct {
	state    protoimpl.MessageState `protogen:"open.v1"`
	JammerId *string                `protobuf:"bytes,1,opt,name=jammer_id,json=jammerId,proto3,oneof" json:"jammer_id,omitempty"`
	// List of active bands. Skycope backend requires the right order of labels.
	// Bands not listed will be switched Off.
	// Example: ["2.4GHz", "5.8GHz"]
	BandsActive []string `protobuf:"bytes,2,rep,name=bands_active,json=bandsActive,proto3" json:"bands_active,omitempty"`
	// Jammer will be stopped after the 'duration' seconds
	Duration      *int32 `protobuf:"varint,3,opt,name=duration,proto3,oneof" json:"duration,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *SetJammerBandsRequest) Reset() {
	*x = SetJammerBandsRequest{}
	mi := &file_jammer_proto_msgTypes[7]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *SetJammerBandsRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*SetJammerBandsRequest) ProtoMessage() {}

func (x *SetJammerBandsRequest) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[7]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use SetJammerBandsRequest.ProtoReflect.Descriptor instead.
func (*SetJammerBandsRequest) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{7}
}

func (x *SetJammerBandsRequest) GetJammerId() string {
	if x != nil && x.JammerId != nil {
		return *x.JammerId
	}
	return ""
}

func (x *SetJammerBandsRequest) GetBandsActive() []string {
	if x != nil {
		return x.BandsActive
	}
	return nil
}

func (x *SetJammerBandsRequest) GetDuration() int32 {
	if x != nil && x.Duration != nil {
		return *x.Duration
	}
	return 0
}

type Band struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	Label         *string                `protobuf:"bytes,1,opt,name=label,proto3,oneof" json:"label,omitempty"`
	Active        *bool                  `protobuf:"varint,2,opt,name=active,proto3,oneof" json:"active,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *Band) Reset() {
	*x = Band{}
	mi := &file_jammer_proto_msgTypes[8]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *Band) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*Band) ProtoMessage() {}

func (x *Band) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[8]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use Band.ProtoReflect.Descriptor instead.
func (*Band) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{8}
}

func (x *Band) GetLabel() string {
	if x != nil && x.Label != nil {
		return *x.Label
	}
	return ""
}

func (x *Band) GetActive() bool {
	if x != nil && x.Active != nil {
		return *x.Active
	}
	return false
}

type BandOption struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	Bands         []string               `protobuf:"bytes,1,rep,name=bands,proto3" json:"bands,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *BandOption) Reset() {
	*x = BandOption{}
	mi := &file_jammer_proto_msgTypes[9]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *BandOption) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*BandOption) ProtoMessage() {}

func (x *BandOption) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[9]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use BandOption.ProtoReflect.Descriptor instead.
func (*BandOption) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{9}
}

func (x *BandOption) GetBands() []string {
	if x != nil {
		return x.Bands
	}
	return nil
}

type JammerTimeoutStatus struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	Started       *timestamppb.Timestamp `protobuf:"bytes,1,opt,name=started,proto3,oneof" json:"started,omitempty"`
	Now           *timestamppb.Timestamp `protobuf:"bytes,2,opt,name=now,proto3,oneof" json:"now,omitempty"`
	Duration      *int32                 `protobuf:"varint,3,opt,name=duration,proto3,oneof" json:"duration,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *JammerTimeoutStatus) Reset() {
	*x = JammerTimeoutStatus{}
	mi := &file_jammer_proto_msgTypes[10]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *JammerTimeoutStatus) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*JammerTimeoutStatus) ProtoMessage() {}

func (x *JammerTimeoutStatus) ProtoReflect() protoreflect.Message {
	mi := &file_jammer_proto_msgTypes[10]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use JammerTimeoutStatus.ProtoReflect.Descriptor instead.
func (*JammerTimeoutStatus) Descriptor() ([]byte, []int) {
	return file_jammer_proto_rawDescGZIP(), []int{10}
}

func (x *JammerTimeoutStatus) GetStarted() *timestamppb.Timestamp {
	if x != nil {
		return x.Started
	}
	return nil
}

func (x *JammerTimeoutStatus) GetNow() *timestamppb.Timestamp {
	if x != nil {
		return x.Now
	}
	return nil
}

func (x *JammerTimeoutStatus) GetDuration() int32 {
	if x != nil && x.Duration != nil {
		return *x.Duration
	}
	return 0
}

var File_jammer_proto protoreflect.FileDescriptor

const file_jammer_proto_rawDesc = "" +
	"\n" +
	"\fjammer.proto\x12\x06api.v1\x1a\x1bbuf/validate/validate.proto\x1a\x1bgoogle/protobuf/empty.proto\x1a\x1fgoogle/protobuf/timestamp.proto\x1a\x10components.proto\"\xb8\x02\n" +
	"\n" +
	"JammerInfo\x12-\n" +
	"\tjammer_id\x18\x01 \x01(\tB\v\xbaH\b\xc8\x01\x01r\x03\xb0\x01\x01H\x00R\bjammerId\x88\x01\x01\x12!\n" +
	"\x05model\x18\x02 \x01(\tB\x06\xbaH\x03\xc8\x01\x01H\x01R\x05model\x88\x01\x01\x12\x1b\n" +
	"\x06serial\x18\x03 \x01(\tH\x02R\x06serial\x88\x01\x01\x12\"\n" +
	"\n" +
	"sw_version\x18\x04 \x01(\tH\x03R\tswVersion\x88\x01\x01\x12*\n" +
	"\tsensor_id\x18\n" +
	" \x01(\tB\b\xbaH\x05r\x03\xb0\x01\x01H\x04R\bsensorId\x88\x01\x01\x12\x1e\n" +
	"\bgroup_id\x18\x0f \x01(\tH\x05R\agroupId\x88\x01\x01B\f\n" +
	"\n" +
	"_jammer_idB\b\n" +
	"\x06_modelB\t\n" +
	"\a_serialB\r\n" +
	"\v_sw_versionB\f\n" +
	"\n" +
	"_sensor_idB\v\n" +
	"\t_group_id\"P\n" +
	"\x0fJammersResponse\x12=\n" +
	"\fjammer_infos\x18\x01 \x03(\v2\x12.api.v1.JammerInfoB\x06\xbaH\x03\xc8\x01\x01R\vjammerInfos\"P\n" +
	"\x11JammerInfoRequest\x12-\n" +
	"\tjammer_id\x18\x01 \x01(\tB\v\xbaH\b\xc8\x01\x01r\x03\xb0\x01\x01H\x00R\bjammerId\x88\x01\x01B\f\n" +
	"\n" +
	"_jammer_id\"W\n" +
	"\x18JammerInfoDynamicRequest\x12-\n" +
	"\tjammer_id\x18\x01 \x01(\tB\v\xbaH\b\xc8\x01\x01r\x03\xb0\x01\x01H\x00R\bjammerId\x88\x01\x01B\f\n" +
	"\n" +
	"_jammer_id\"\xdb\x04\n" +
	"\x19JammerInfoDynamicResponse\x12'\n" +
	"\bdisabled\x18\x05 \x01(\bB\x06\xbaH\x03\xc8\x01\x01H\x00R\bdisabled\x88\x01\x01\x12!\n" +
	"\x05state\x18\x06 \x01(\tB\x06\xbaH\x03\xc8\x01\x01H\x01R\x05state\x88\x01\x01\x12<\n" +
	"\bposition\x18\a \x01(\v2\x13.api.v1.GeoPositionB\x06\xbaH\x03\xc8\x01\x01H\x02R\bposition\x88\x01\x01\x12I\n" +
	"\rposition_mode\x18\b \x01(\x0e2\x17.api.v1.GeoPositionModeB\x06\xbaH\x03\xc8\x01\x01H\x03R\fpositionMode\x88\x01\x01\x12:\n" +
	"\bworkzone\x18\t \x03(\v2\x16.api.v1.WorkzoneSectorB\x06\xbaH\x03\xc8\x01\x01R\bworkzone\x12*\n" +
	"\x05bands\x18\v \x03(\v2\f.api.v1.BandB\x06\xbaH\x03\xc8\x01\x01R\x05bands\x125\n" +
	"\fband_options\x18\f \x03(\v2\x12.api.v1.BandOptionR\vbandOptions\x12,\n" +
	"\ahw_info\x18\r \x01(\v2\x0e.api.v1.HwInfoH\x04R\x06hwInfo\x88\x01\x01\x12G\n" +
	"\x0etimeout_status\x18\x0e \x01(\v2\x1b.api.v1.JammerTimeoutStatusH\x05R\rtimeoutStatus\x88\x01\x01B\v\n" +
	"\t_disabledB\b\n" +
	"\x06_stateB\v\n" +
	"\t_positionB\x10\n" +
	"\x0e_position_modeB\n" +
	"\n" +
	"\b_hw_infoB\x11\n" +
	"\x0f_timeout_status\"R\n" +
	"\x0eGroupsResponse\x12@\n" +
	"\rjammer_groups\x18\x01 \x03(\v2\x13.api.v1.JammerGroupB\x06\xbaH\x03\xc8\x01\x01R\fjammerGroups\"q\n" +
	"\vJammerGroup\x12+\n" +
	"\bgroup_id\x18\x01 \x01(\tB\v\xbaH\b\xc8\x01\x01r\x03\xb0\x01\x01H\x00R\agroupId\x88\x01\x01\x12\x1f\n" +
	"\x04name\x18\x02 \x01(\tB\x06\xbaH\x03\xc8\x01\x01H\x01R\x04name\x88\x01\x01B\v\n" +
	"\t_group_idB\a\n" +
	"\x05_name\"\xb5\x01\n" +
	"\x15SetJammerBandsRequest\x12-\n" +
	"\tjammer_id\x18\x01 \x01(\tB\v\xbaH\b\xc8\x01\x01r\x03\xb0\x01\x01H\x00R\bjammerId\x88\x01\x01\x12)\n" +
	"\fbands_active\x18\x02 \x03(\tB\x06\xbaH\x03\xc8\x01\x01R\vbandsActive\x12'\n" +
	"\bduration\x18\x03 \x01(\x05B\x06\xbaH\x03\xc8\x01\x01H\x01R\bduration\x88\x01\x01B\f\n" +
	"\n" +
	"_jammer_idB\v\n" +
	"\t_duration\"c\n" +
	"\x04Band\x12!\n" +
	"\x05label\x18\x01 \x01(\tB\x06\xbaH\x03\xc8\x01\x01H\x00R\x05label\x88\x01\x01\x12#\n" +
	"\x06active\x18\x02 \x01(\bB\x06\xbaH\x03\xc8\x01\x01H\x01R\x06active\x88\x01\x01B\b\n" +
	"\x06_labelB\t\n" +
	"\a_active\"*\n" +
	"\n" +
	"BandOption\x12\x1c\n" +
	"\x05bands\x18\x01 \x03(\tB\x06\xbaH\x03\xc8\x01\x01R\x05bands\"\xdd\x01\n" +
	"\x13JammerTimeoutStatus\x12A\n" +
	"\astarted\x18\x01 \x01(\v2\x1a.google.protobuf.TimestampB\x06\xbaH\x03\xc8\x01\x01H\x00R\astarted\x88\x01\x01\x129\n" +
	"\x03now\x18\x02 \x01(\v2\x1a.google.protobuf.TimestampB\x06\xbaH\x03\xc8\x01\x01H\x01R\x03now\x88\x01\x01\x12'\n" +
	"\bduration\x18\x03 \x01(\x05B\x06\xbaH\x03\xc8\x01\x01H\x02R\bduration\x88\x01\x01B\n" +
	"\n" +
	"\b_startedB\x06\n" +
	"\x04_nowB\v\n" +
	"\t_duration2\xab\x02\n" +
	"\rJammerService\x12<\n" +
	"\aJammers\x12\x16.google.protobuf.Empty\x1a\x17.api.v1.JammersResponse\"\x00\x12U\n" +
	"\x11JammerInfoDynamic\x12\x19.api.v1.JammerInfoRequest\x1a!.api.v1.JammerInfoDynamicResponse\"\x000\x01\x12:\n" +
	"\x06Groups\x12\x16.google.protobuf.Empty\x1a\x16.api.v1.GroupsResponse\"\x00\x12I\n" +
	"\x0eSetJammerBands\x12\x1d.api.v1.SetJammerBandsRequest\x1a\x16.google.protobuf.Empty\"\x00B\x83\x01\n" +
	"\n" +
	"com.api.v1B\vJammerProtoP\x01Z/dds-provider/internal/generated/api/proto;apiv1\xa2\x02\x03AXX\xaa\x02\x06Api.V1\xca\x02\x06Api\\V1\xe2\x02\x12Api\\V1\\GPBMetadata\xea\x02\aApi::V1b\x06proto3"

var (
	file_jammer_proto_rawDescOnce sync.Once
	file_jammer_proto_rawDescData []byte
)

func file_jammer_proto_rawDescGZIP() []byte {
	file_jammer_proto_rawDescOnce.Do(func() {
		file_jammer_proto_rawDescData = protoimpl.X.CompressGZIP(unsafe.Slice(unsafe.StringData(file_jammer_proto_rawDesc), len(file_jammer_proto_rawDesc)))
	})
	return file_jammer_proto_rawDescData
}

var file_jammer_proto_msgTypes = make([]protoimpl.MessageInfo, 11)
var file_jammer_proto_goTypes = []any{
	(*JammerInfo)(nil),                // 0: api.v1.JammerInfo
	(*JammersResponse)(nil),           // 1: api.v1.JammersResponse
	(*JammerInfoRequest)(nil),         // 2: api.v1.JammerInfoRequest
	(*JammerInfoDynamicRequest)(nil),  // 3: api.v1.JammerInfoDynamicRequest
	(*JammerInfoDynamicResponse)(nil), // 4: api.v1.JammerInfoDynamicResponse
	(*GroupsResponse)(nil),            // 5: api.v1.GroupsResponse
	(*JammerGroup)(nil),               // 6: api.v1.JammerGroup
	(*SetJammerBandsRequest)(nil),     // 7: api.v1.SetJammerBandsRequest
	(*Band)(nil),                      // 8: api.v1.Band
	(*BandOption)(nil),                // 9: api.v1.BandOption
	(*JammerTimeoutStatus)(nil),       // 10: api.v1.JammerTimeoutStatus
	(*GeoPosition)(nil),               // 11: api.v1.GeoPosition
	(GeoPositionMode)(0),              // 12: api.v1.GeoPositionMode
	(*WorkzoneSector)(nil),            // 13: api.v1.WorkzoneSector
	(*HwInfo)(nil),                    // 14: api.v1.HwInfo
	(*timestamppb.Timestamp)(nil),     // 15: google.protobuf.Timestamp
	(*emptypb.Empty)(nil),             // 16: google.protobuf.Empty
}
var file_jammer_proto_depIdxs = []int32{
	0,  // 0: api.v1.JammersResponse.jammer_infos:type_name -> api.v1.JammerInfo
	11, // 1: api.v1.JammerInfoDynamicResponse.position:type_name -> api.v1.GeoPosition
	12, // 2: api.v1.JammerInfoDynamicResponse.position_mode:type_name -> api.v1.GeoPositionMode
	13, // 3: api.v1.JammerInfoDynamicResponse.workzone:type_name -> api.v1.WorkzoneSector
	8,  // 4: api.v1.JammerInfoDynamicResponse.bands:type_name -> api.v1.Band
	9,  // 5: api.v1.JammerInfoDynamicResponse.band_options:type_name -> api.v1.BandOption
	14, // 6: api.v1.JammerInfoDynamicResponse.hw_info:type_name -> api.v1.HwInfo
	10, // 7: api.v1.JammerInfoDynamicResponse.timeout_status:type_name -> api.v1.JammerTimeoutStatus
	6,  // 8: api.v1.GroupsResponse.jammer_groups:type_name -> api.v1.JammerGroup
	15, // 9: api.v1.JammerTimeoutStatus.started:type_name -> google.protobuf.Timestamp
	15, // 10: api.v1.JammerTimeoutStatus.now:type_name -> google.protobuf.Timestamp
	16, // 11: api.v1.JammerService.Jammers:input_type -> google.protobuf.Empty
	2,  // 12: api.v1.JammerService.JammerInfoDynamic:input_type -> api.v1.JammerInfoRequest
	16, // 13: api.v1.JammerService.Groups:input_type -> google.protobuf.Empty
	7,  // 14: api.v1.JammerService.SetJammerBands:input_type -> api.v1.SetJammerBandsRequest
	1,  // 15: api.v1.JammerService.Jammers:output_type -> api.v1.JammersResponse
	4,  // 16: api.v1.JammerService.JammerInfoDynamic:output_type -> api.v1.JammerInfoDynamicResponse
	5,  // 17: api.v1.JammerService.Groups:output_type -> api.v1.GroupsResponse
	16, // 18: api.v1.JammerService.SetJammerBands:output_type -> google.protobuf.Empty
	15, // [15:19] is the sub-list for method output_type
	11, // [11:15] is the sub-list for method input_type
	11, // [11:11] is the sub-list for extension type_name
	11, // [11:11] is the sub-list for extension extendee
	0,  // [0:11] is the sub-list for field type_name
}

func init() { file_jammer_proto_init() }
func file_jammer_proto_init() {
	if File_jammer_proto != nil {
		return
	}
	file_components_proto_init()
	file_jammer_proto_msgTypes[0].OneofWrappers = []any{}
	file_jammer_proto_msgTypes[2].OneofWrappers = []any{}
	file_jammer_proto_msgTypes[3].OneofWrappers = []any{}
	file_jammer_proto_msgTypes[4].OneofWrappers = []any{}
	file_jammer_proto_msgTypes[6].OneofWrappers = []any{}
	file_jammer_proto_msgTypes[7].OneofWrappers = []any{}
	file_jammer_proto_msgTypes[8].OneofWrappers = []any{}
	file_jammer_proto_msgTypes[10].OneofWrappers = []any{}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: unsafe.Slice(unsafe.StringData(file_jammer_proto_rawDesc), len(file_jammer_proto_rawDesc)),
			NumEnums:      0,
			NumMessages:   11,
			NumExtensions: 0,
			NumServices:   1,
		},
		GoTypes:           file_jammer_proto_goTypes,
		DependencyIndexes: file_jammer_proto_depIdxs,
		MessageInfos:      file_jammer_proto_msgTypes,
	}.Build()
	File_jammer_proto = out.File
	file_jammer_proto_goTypes = nil
	file_jammer_proto_depIdxs = nil
}
