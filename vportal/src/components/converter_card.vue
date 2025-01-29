<template>
  <div class="converter-card">
    <p><strong>Конвертор #{{ converter.id }}</strong></p>
    <p>Файл: {{ converter.currentFile || 'Нет файла' }}</p>
    <p>Размер: {{ converter.fileSize || '—' }}</p>
    <p>Исходный формат: {{ converter.sourceFormat || '—' }}</p>
    <p>Целевой формат: {{ converter.targetFormat || '—' }}</p>
    <p>Статус: {{ converter.status }}</p>
  </div>
</template>

<script>
export default {
  name: 'ConverterCard',
  props: {
    converter: {
      type: Object,
      required: true,
    },
  },
};
</script>

<style scoped>
.converter-card {
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 16px;
  margin: 10px;
  width: 300px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  text-align: left;
}
</style>