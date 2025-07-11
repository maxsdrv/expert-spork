// Code generated by protoc-gen-go. DO NOT EDIT.
// versions:
// 	protoc-gen-go v1.36.6
// 	protoc        (unknown)
// source: camera.proto

package apiv1

import (
	_ "buf.build/gen/go/bufbuild/protovalidate/protocolbuffers/go/buf/validate"
	protoreflect "google.golang.org/protobuf/reflect/protoreflect"
	protoimpl "google.golang.org/protobuf/runtime/protoimpl"
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

type CameraIdRequest struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	SensorId      *string                `protobuf:"bytes,1,opt,name=sensor_id,json=sensorId,proto3,oneof" json:"sensor_id,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *CameraIdRequest) Reset() {
	*x = CameraIdRequest{}
	mi := &file_camera_proto_msgTypes[0]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *CameraIdRequest) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CameraIdRequest) ProtoMessage() {}

func (x *CameraIdRequest) ProtoReflect() protoreflect.Message {
	mi := &file_camera_proto_msgTypes[0]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CameraIdRequest.ProtoReflect.Descriptor instead.
func (*CameraIdRequest) Descriptor() ([]byte, []int) {
	return file_camera_proto_rawDescGZIP(), []int{0}
}

func (x *CameraIdRequest) GetSensorId() string {
	if x != nil && x.SensorId != nil {
		return *x.SensorId
	}
	return ""
}

type CameraIdResponse struct {
	state         protoimpl.MessageState `protogen:"open.v1"`
	CameraId      *string                `protobuf:"bytes,1,opt,name=camera_id,json=cameraId,proto3,oneof" json:"camera_id,omitempty"`
	unknownFields protoimpl.UnknownFields
	sizeCache     protoimpl.SizeCache
}

func (x *CameraIdResponse) Reset() {
	*x = CameraIdResponse{}
	mi := &file_camera_proto_msgTypes[1]
	ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
	ms.StoreMessageInfo(mi)
}

func (x *CameraIdResponse) String() string {
	return protoimpl.X.MessageStringOf(x)
}

func (*CameraIdResponse) ProtoMessage() {}

func (x *CameraIdResponse) ProtoReflect() protoreflect.Message {
	mi := &file_camera_proto_msgTypes[1]
	if x != nil {
		ms := protoimpl.X.MessageStateOf(protoimpl.Pointer(x))
		if ms.LoadMessageInfo() == nil {
			ms.StoreMessageInfo(mi)
		}
		return ms
	}
	return mi.MessageOf(x)
}

// Deprecated: Use CameraIdResponse.ProtoReflect.Descriptor instead.
func (*CameraIdResponse) Descriptor() ([]byte, []int) {
	return file_camera_proto_rawDescGZIP(), []int{1}
}

func (x *CameraIdResponse) GetCameraId() string {
	if x != nil && x.CameraId != nil {
		return *x.CameraId
	}
	return ""
}

var File_camera_proto protoreflect.FileDescriptor

const file_camera_proto_rawDesc = "" +
	"\n" +
	"\fcamera.proto\x12\x06api.v1\x1a\x1bbuf/validate/validate.proto\x1a\x10components.proto\"N\n" +
	"\x0fCameraIdRequest\x12-\n" +
	"\tsensor_id\x18\x01 \x01(\tB\v\xbaH\b\xc8\x01\x01r\x03\xb0\x01\x01H\x00R\bsensorId\x88\x01\x01B\f\n" +
	"\n" +
	"_sensor_id\"J\n" +
	"\x10CameraIdResponse\x12(\n" +
	"\tcamera_id\x18\x01 \x01(\tB\x06\xbaH\x03\xc8\x01\x01H\x00R\bcameraId\x88\x01\x01B\f\n" +
	"\n" +
	"_camera_id2P\n" +
	"\rCameraService\x12?\n" +
	"\bCameraId\x12\x17.api.v1.CameraIdRequest\x1a\x18.api.v1.CameraIdResponse\"\x00B\x83\x01\n" +
	"\n" +
	"com.api.v1B\vCameraProtoP\x01Z/dds-provider/internal/generated/api/proto;apiv1\xa2\x02\x03AXX\xaa\x02\x06Api.V1\xca\x02\x06Api\\V1\xe2\x02\x12Api\\V1\\GPBMetadata\xea\x02\aApi::V1b\x06proto3"

var (
	file_camera_proto_rawDescOnce sync.Once
	file_camera_proto_rawDescData []byte
)

func file_camera_proto_rawDescGZIP() []byte {
	file_camera_proto_rawDescOnce.Do(func() {
		file_camera_proto_rawDescData = protoimpl.X.CompressGZIP(unsafe.Slice(unsafe.StringData(file_camera_proto_rawDesc), len(file_camera_proto_rawDesc)))
	})
	return file_camera_proto_rawDescData
}

var file_camera_proto_msgTypes = make([]protoimpl.MessageInfo, 2)
var file_camera_proto_goTypes = []any{
	(*CameraIdRequest)(nil),  // 0: api.v1.CameraIdRequest
	(*CameraIdResponse)(nil), // 1: api.v1.CameraIdResponse
}
var file_camera_proto_depIdxs = []int32{
	0, // 0: api.v1.CameraService.CameraId:input_type -> api.v1.CameraIdRequest
	1, // 1: api.v1.CameraService.CameraId:output_type -> api.v1.CameraIdResponse
	1, // [1:2] is the sub-list for method output_type
	0, // [0:1] is the sub-list for method input_type
	0, // [0:0] is the sub-list for extension type_name
	0, // [0:0] is the sub-list for extension extendee
	0, // [0:0] is the sub-list for field type_name
}

func init() { file_camera_proto_init() }
func file_camera_proto_init() {
	if File_camera_proto != nil {
		return
	}
	file_components_proto_init()
	file_camera_proto_msgTypes[0].OneofWrappers = []any{}
	file_camera_proto_msgTypes[1].OneofWrappers = []any{}
	type x struct{}
	out := protoimpl.TypeBuilder{
		File: protoimpl.DescBuilder{
			GoPackagePath: reflect.TypeOf(x{}).PkgPath(),
			RawDescriptor: unsafe.Slice(unsafe.StringData(file_camera_proto_rawDesc), len(file_camera_proto_rawDesc)),
			NumEnums:      0,
			NumMessages:   2,
			NumExtensions: 0,
			NumServices:   1,
		},
		GoTypes:           file_camera_proto_goTypes,
		DependencyIndexes: file_camera_proto_depIdxs,
		MessageInfos:      file_camera_proto_msgTypes,
	}.Build()
	File_camera_proto = out.File
	file_camera_proto_goTypes = nil
	file_camera_proto_depIdxs = nil
}
