def split_content(content, count):
    segment_size = len(content)//count + 1
    segments = []

    last_pos = 0
    pos = segment_size
    while pos < len(content):
        segments.append((last_pos, pos-1, content[last_pos:pos]))
        last_pos = pos
        pos += segment_size

    segments.append((last_pos, len(content)-1, content[last_pos:]))

    return segments
