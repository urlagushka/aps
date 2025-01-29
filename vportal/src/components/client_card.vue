<template>
  <div class="client-card">
    <p><strong>Клиент #{{ client.id }}</strong></p>
    <p><i class="fas fa-file"></i> Текущий файл: {{ client.fileName || 'Нет файла' }}</p>
    <p><i class="fas fa-weight-hanging"></i> Размер файла: {{ formatFileSize(client.fileSize) }}</p>
    <p><i class="fas fa-file-import"></i> Исходный формат: {{ client.sourceFormat || '—' }}</p>
    <p><i class="fas fa-file-export"></i> Целевой формат: {{ client.targetFormat || '—' }}</p>
    <p :class="['status', client.status]">{{ getStatusText(client.status) }}</p>
    <div class="actions">
      <button @click="deleteClient">Удалить</button>
      <button @click="editClient">Редактировать</button>
    </div>
  </div>
</template>

<script>
export default {
  name: 'ClientCard',
  props: {
    client: {
      type: Object,
      required: true,
    },
  },
  methods: {
    formatFileSize(size) {
      if (!size) return '—';
      if (size < 1024) return `${size} КБ`;
      if (size < 1024 * 1024) return `${(size / 1024).toFixed(2)} МБ`;
      return `${(size / (1024 * 1024)).toFixed(2)} ГБ`;
    },
    getStatusText(status) {
      const statusMap = {
        ожидание: 'Ожидание',
        обработка: 'Обработка',
        окончание: 'Завершено',
      };
      return statusMap[status] || status;
    },
    deleteClient() {
      this.$emit('delete-client', this.client.id);
    },
    editClient() {
      this.$emit('edit-client', this.client.id);
    },
  },
};
</script>

<style scoped>
.client-card {
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 16px;
  margin: 10px;
  width: 300px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  text-align: left;
}

.status {
  padding: 4px 8px;
  border-radius: 4px;
  font-size: 14px;
  font-weight: bold;
  text-align: center;
  margin-top: 10px;
}

.status.ожидание {
  background-color: #fff3e0; /* Оранжевый фон для статуса "ожидание" */
  color: #ef6c00; /* Оранжевый текст */
}

.status.обработка {
  background-color: #e3f2fd; /* Голубой фон для статуса "обработка" */
  color: #1976d2; /* Синий текст */
}

.status.окончание {
  background-color: #e8f5e9; /* Зеленый фон для статуса "окончание" */
  color: #2e7d32; /* Зеленый текст */
}

.actions {
  margin-top: 10px;
  display: flex;
  gap: 8px;
}

button {
  padding: 5px 10px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  background-color: #1976d2;
  color: white;
}

button:hover {
  opacity: 0.8;
}

button:active {
  opacity: 0.6;
}
</style>