/* eslint import/prefer-default-export: "off" */
export function getDataView(data) {
  const { buffer, byteOffset, byteLength } = data;
  return new DataView(buffer, byteOffset, byteLength);
}
